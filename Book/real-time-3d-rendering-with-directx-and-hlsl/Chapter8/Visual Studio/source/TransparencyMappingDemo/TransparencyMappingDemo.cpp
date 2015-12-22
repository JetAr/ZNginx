﻿#include "TransparencyMappingDemo.h"
#include "TransparencyMappingMaterial.h"
#include "Game.h"
#include "GameException.h"
#include "VectorHelper.h"
#include "MatrixHelper.h"
#include "ColorHelper.h"
#include "Camera.h"
#include "Model.h"
#include "Mesh.h"
#include "Utility.h"
#include "PointLight.h"
#include "Keyboard.h"
#include <WICTextureLoader.h>
#include "BlendStates.h"
#include "ProxyModel.h"
#include "RenderStateHelper.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <sstream>

namespace Rendering
{
RTTI_DEFINITIONS(TransparencyMappingDemo)

const float TransparencyMappingDemo::LightModulationRate = UCHAR_MAX;
const float TransparencyMappingDemo::LightMovementRate = 10.0f;

TransparencyMappingDemo::TransparencyMappingDemo(Game& game, Camera& camera)
    : DrawableGameComponent(game, camera), mEffect(nullptr), mMaterial(nullptr), mColorTextureShaderResourceView(nullptr),
      mTransparencyMapShaderResourceView(nullptr), mVertexBuffer(nullptr), mVertexCount(0),
      mKeyboard(nullptr), mAmbientColor(1, 1, 1, 0), mPointLight(nullptr),
      mSpecularColor(1.0f, 1.0f, 1.0f, 1.0f), mSpecularPower(25.0f), mWorldMatrix(MatrixHelper::Identity), mProxyModel(nullptr),
      mRenderStateHelper(nullptr), mSpriteBatch(nullptr), mSpriteFont(nullptr), mTextPosition(0.0f, 40.0f)
{
}

TransparencyMappingDemo::~TransparencyMappingDemo()
{
    BlendStates::Release();

    DeleteObject(mSpriteFont);
    DeleteObject(mSpriteBatch);
    DeleteObject(mRenderStateHelper);
    DeleteObject(mProxyModel);
    DeleteObject(mPointLight);
    ReleaseObject(mTransparencyMapShaderResourceView);
    ReleaseObject(mColorTextureShaderResourceView);
    DeleteObject(mMaterial);
    DeleteObject(mEffect);
    ReleaseObject(mVertexBuffer);
}

void TransparencyMappingDemo::Initialize()
{
    SetCurrentDirectory(Utility::ExecutableDirectory().c_str());

    // Initialize the material
    mEffect = new Effect(*mGame);
    mEffect->CompileFromFile(L"Content\\Effects\\TransparencyMapping.fx");

    mMaterial = new TransparencyMappingMaterial();
    mMaterial->Initialize(*mEffect);

    VertexPositionTextureNormal vertices[] =
    {
        VertexPositionTextureNormal(XMFLOAT4(-0.5f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f), Vector3Helper::Backward),
        VertexPositionTextureNormal(XMFLOAT4(-0.5f, 1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f), Vector3Helper::Backward),
        VertexPositionTextureNormal(XMFLOAT4(0.5f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f), Vector3Helper::Backward),

        VertexPositionTextureNormal(XMFLOAT4(-0.5f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f), Vector3Helper::Backward),
        VertexPositionTextureNormal(XMFLOAT4(0.5f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f), Vector3Helper::Backward),
        VertexPositionTextureNormal(XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f), Vector3Helper::Backward),
    };

    mVertexCount = ARRAYSIZE(vertices);
    mMaterial->CreateVertexBuffer(mGame->Direct3DDevice(), vertices, mVertexCount, &mVertexBuffer);

    std::wstring textureName = L"Content\\Textures\\Checkerboard.png";
    HRESULT hr = DirectX::CreateWICTextureFromFile(mGame->Direct3DDevice(), mGame->Direct3DDeviceContext(), textureName.c_str(), nullptr, &mColorTextureShaderResourceView);
    if (FAILED(hr))
    {
        throw GameException("CreateWICTextureFromFile() failed.", hr);
    }

    textureName = L"Content\\Textures\\AlphaMask_32bpp.png";
    hr = DirectX::CreateWICTextureFromFile(mGame->Direct3DDevice(), mGame->Direct3DDeviceContext(), textureName.c_str(), nullptr, &mTransparencyMapShaderResourceView);
    if (FAILED(hr))
    {
        throw GameException("CreateWICTextureFromFile() failed.", hr);
    }

    mPointLight = new PointLight(*mGame);
    mPointLight->SetRadius(500.0f);
    mPointLight->SetPosition(5.0f, 0.0f, 10.0f);

    mKeyboard = (Keyboard*)mGame->Services().GetService(Keyboard::TypeIdClass());
    assert(mKeyboard != nullptr);

    mProxyModel = new ProxyModel(*mGame, *mCamera, "Content\\Models\\PointLightProxy.obj", 0.5f);
    mProxyModel->Initialize();

    mRenderStateHelper = new RenderStateHelper(*mGame);
    BlendStates::Initialize(mGame->Direct3DDevice());

    mSpriteBatch = new SpriteBatch(mGame->Direct3DDeviceContext());
    mSpriteFont = new SpriteFont(mGame->Direct3DDevice(), L"Content\\Fonts\\Arial_14_Regular.spritefont");

    XMStoreFloat4x4(&mWorldMatrix, XMMatrixScaling(10.0f, 10.0f, 10.0f));
}

void TransparencyMappingDemo::Update(const GameTime& gameTime)
{
    UpdateAmbientLight(gameTime);
    UpdatePointLight(gameTime);
    UpdateSpecularLight(gameTime);

    mProxyModel->Update(gameTime);
}

void TransparencyMappingDemo::Draw(const GameTime& gameTime)
{
    ID3D11DeviceContext* direct3DDeviceContext = mGame->Direct3DDeviceContext();
    direct3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    Pass* pass = mMaterial->CurrentTechnique()->Passes().at(0);
    ID3D11InputLayout* inputLayout = mMaterial->InputLayouts().at(pass);
    direct3DDeviceContext->IASetInputLayout(inputLayout);

    UINT stride = mMaterial->VertexSize();
    UINT offset = 0;
    direct3DDeviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

    XMMATRIX worldMatrix = XMLoadFloat4x4(&mWorldMatrix);
    XMMATRIX wvp = worldMatrix * mCamera->ViewMatrix() * mCamera->ProjectionMatrix();
    XMVECTOR ambientColor = XMLoadColor(&mAmbientColor);
    XMVECTOR specularColor = XMLoadColor(&mSpecularColor);

    mMaterial->WorldViewProjection() << wvp;
    mMaterial->World() << worldMatrix;
    mMaterial->SpecularColor() << specularColor;
    mMaterial->SpecularPower() << mSpecularPower;
    mMaterial->AmbientColor() << ambientColor;
    mMaterial->LightColor() << mPointLight->ColorVector();
    mMaterial->LightPosition() << mPointLight->PositionVector();
    mMaterial->LightRadius() << mPointLight->Radius();
    mMaterial->ColorTexture() << mColorTextureShaderResourceView;
    mMaterial->TransparencyMap() << mTransparencyMapShaderResourceView;
    mMaterial->CameraPosition() << mCamera->PositionVector();

    pass->Apply(0, direct3DDeviceContext);

    mRenderStateHelper->SaveBlendState();
    direct3DDeviceContext->OMSetBlendState(BlendStates::AlphaBlending, 0, 0xFFFFFFFF);
    direct3DDeviceContext->Draw(mVertexCount, 0);
    mRenderStateHelper->RestoreBlendState();

    mProxyModel->Draw(gameTime);

    mRenderStateHelper->SaveAll();
    mSpriteBatch->Begin();

    std::wostringstream helpLabel;
    helpLabel << L"Ambient Intensity (+PgUp/-PgDn): " << mAmbientColor.a << "\n";
    helpLabel << L"Point Light Intensity (+Home/-End): " << mPointLight->Color().a << "\n";
    helpLabel << L"Specular Power (+Insert/-Delete): " << mSpecularPower << "\n";
    helpLabel << L"Move Point Light (8/2, 4/6, 3/9)\n";

    mSpriteFont->DrawString(mSpriteBatch, helpLabel.str().c_str(), mTextPosition);

    mSpriteBatch->End();
    mRenderStateHelper->RestoreAll();
}

void TransparencyMappingDemo::UpdateSpecularLight(const GameTime& gameTime)
{
    static float specularIntensity = mSpecularPower;

    if (mKeyboard != nullptr)
    {
        if (mKeyboard->IsKeyDown(DIK_INSERT) && specularIntensity < UCHAR_MAX)
        {
            specularIntensity += LightModulationRate * (float)gameTime.ElapsedGameTime();
            specularIntensity = XMMin<float>(specularIntensity, UCHAR_MAX);

            mSpecularPower = specularIntensity;;
        }

        if (mKeyboard->IsKeyDown(DIK_DELETE) && specularIntensity > 0)
        {
            specularIntensity -= LightModulationRate * (float)gameTime.ElapsedGameTime();
            specularIntensity = XMMax<float>(specularIntensity, 0);

            mSpecularPower = specularIntensity;
        }
    }
}

void TransparencyMappingDemo::UpdatePointLight(const GameTime& gameTime)
{
    static float pointLightIntensity = mPointLight->Color().a;
    float elapsedTime = (float)gameTime.ElapsedGameTime();

    // Update point light intensity
    if (mKeyboard->IsKeyDown(DIK_HOME) && pointLightIntensity < UCHAR_MAX)
    {
        pointLightIntensity += LightModulationRate * elapsedTime;

        XMCOLOR pointLightLightColor = mPointLight->Color();
        pointLightLightColor.a = (UCHAR)XMMin<float>(pointLightIntensity, UCHAR_MAX);
        mPointLight->SetColor(pointLightLightColor);
    }
    if (mKeyboard->IsKeyDown(DIK_END) && pointLightIntensity > 0)
    {
        pointLightIntensity -= LightModulationRate * elapsedTime;

        XMCOLOR pointLightLightColor = mPointLight->Color();
        pointLightLightColor.a = (UCHAR)XMMax<float>(pointLightIntensity, 0.0f);
        mPointLight->SetColor(pointLightLightColor);
    }

    // Move point light
    XMFLOAT3 movementAmount = Vector3Helper::Zero;
    if (mKeyboard != nullptr)
    {
        if (mKeyboard->IsKeyDown(DIK_NUMPAD4))
        {
            movementAmount.x = -1.0f;
        }

        if (mKeyboard->IsKeyDown(DIK_NUMPAD6))
        {
            movementAmount.x = 1.0f;
        }

        if (mKeyboard->IsKeyDown(DIK_NUMPAD9))
        {
            movementAmount.y = 1.0f;
        }

        if (mKeyboard->IsKeyDown(DIK_NUMPAD3))
        {
            movementAmount.y = -1.0f;
        }

        if (mKeyboard->IsKeyDown(DIK_NUMPAD8))
        {
            movementAmount.z = -1.0f;
        }

        if (mKeyboard->IsKeyDown(DIK_NUMPAD2))
        {
            movementAmount.z = 1.0f;
        }
    }

    XMVECTOR movement = XMLoadFloat3(&movementAmount) * LightMovementRate * elapsedTime;
    mPointLight->SetPosition(mPointLight->PositionVector() + movement);
    mProxyModel->SetPosition(mPointLight->Position());
}

void TransparencyMappingDemo::UpdateAmbientLight(const GameTime& gameTime)
{
    static float ambientIntensity = mAmbientColor.a;

    if (mKeyboard != nullptr)
    {
        if (mKeyboard->IsKeyDown(DIK_PGUP) && ambientIntensity < UCHAR_MAX)
        {
            ambientIntensity += LightModulationRate * (float)gameTime.ElapsedGameTime();
            mAmbientColor.a = (UCHAR)XMMin<float>(ambientIntensity, UCHAR_MAX);
        }

        if (mKeyboard->IsKeyDown(DIK_PGDN) && ambientIntensity > 0)
        {
            ambientIntensity -= LightModulationRate * (float)gameTime.ElapsedGameTime();
            mAmbientColor.a = (UCHAR)XMMax<float>(ambientIntensity, 0);
        }
    }
}
}
