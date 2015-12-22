﻿#include "Frustum.h"

namespace Library
{
Frustum::Frustum(CXMMATRIX matrix)
    : mMatrix(), mCorners(), mPlanes()
{
    SetMatrix(matrix);
}

const XMFLOAT4& Frustum::Near() const
{
    return mPlanes[FrustumPlaneNear];
}

const XMFLOAT4& Frustum::Far() const
{
    return mPlanes[FrustumPlaneFar];
}

const XMFLOAT4& Frustum::Left() const
{
    return mPlanes[FrustumPlaneLeft];
}

const XMFLOAT4& Frustum::Right() const
{
    return mPlanes[FrustumPlaneRight];
}

const XMFLOAT4& Frustum::Top() const
{
    return mPlanes[FrustumPlaneTop];
}

const XMFLOAT4& Frustum::Bottom() const
{
    return mPlanes[FrustumPlaneBottom];
}

XMVECTOR Frustum::NearVector() const
{
    return XMLoadFloat4(&mPlanes[FrustumPlaneNear]);
}

XMVECTOR Frustum::FarVector() const
{
    return XMLoadFloat4(&mPlanes[FrustumPlaneFar]);
}

XMVECTOR Frustum::LeftVector() const
{
    return XMLoadFloat4(&mPlanes[FrustumPlaneLeft]);
}

XMVECTOR Frustum::RightVector() const
{
    return XMLoadFloat4(&mPlanes[FrustumPlaneRight]);
}

XMVECTOR Frustum::TopVector() const
{
    return XMLoadFloat4(&mPlanes[FrustumPlaneTop]);
}

XMVECTOR Frustum::BottomVector() const
{
    return XMLoadFloat4(&mPlanes[FrustumPlaneBottom]);
}

const XMFLOAT3* Frustum::Corners() const
{
    return mCorners;
}

XMMATRIX Frustum::Matrix() const
{
    return XMLoadFloat4x4(&mMatrix);
}

void Frustum::SetMatrix(CXMMATRIX matrix)
{
    XMFLOAT4X4 m;
    XMStoreFloat4x4(&m, matrix);
    SetMatrix(m);
}

void Frustum::SetMatrix(const XMFLOAT4X4& matrix)
{
    mMatrix = matrix;

    mPlanes[0].x = -matrix._13;
    mPlanes[0].y = -matrix._23;
    mPlanes[0].z = -matrix._33;
    mPlanes[0].w = -matrix._43;

    mPlanes[1].x = -matrix._14 + matrix._13;
    mPlanes[1].y = -matrix._24 + matrix._23;
    mPlanes[1].z = -matrix._34 + matrix._33;
    mPlanes[1].w = -matrix._44 + matrix._43;

    mPlanes[2].x = -matrix._14 - matrix._11;
    mPlanes[2].y = -matrix._24 - matrix._21;
    mPlanes[2].z = -matrix._34 - matrix._31;
    mPlanes[2].w = -matrix._44 - matrix._41;

    mPlanes[3].x = -matrix._14 + matrix._11;
    mPlanes[3].y = -matrix._24 + matrix._21;
    mPlanes[3].z = -matrix._34 + matrix._31;
    mPlanes[3].w = -matrix._44 + matrix._41;

    mPlanes[4].x = -matrix._14 + matrix._12;
    mPlanes[4].y = -matrix._24 + matrix._22;
    mPlanes[4].z = -matrix._34 + matrix._32;
    mPlanes[4].w = -matrix._44 + matrix._42;

    mPlanes[5].x = -matrix._14 - matrix._12;
    mPlanes[5].y = -matrix._24 - matrix._22;
    mPlanes[5].z = -matrix._34 - matrix._32;
    mPlanes[5].w = -matrix._44 - matrix._42;

    for (int i = 0; i < 6; i++)
    {
        XMVECTOR vector = XMVectorSet(mPlanes[i].x, mPlanes[i].y, mPlanes[i].z, mPlanes[i].w);
        XMVECTOR length = XMVector3Length(vector);

        XMStoreFloat4(&mPlanes[i], XMVectorDivide(vector, length));
    }

    Ray ray = ComputeIntersectionLine(XMLoadFloat4(&mPlanes[0]), XMLoadFloat4(&mPlanes[2]));
    XMStoreFloat3(&mCorners[0], ComputeIntersection(XMLoadFloat4(&mPlanes[4]), ray));
    XMStoreFloat3(&mCorners[3], ComputeIntersection(XMLoadFloat4(&mPlanes[5]), ray));

    ray = ComputeIntersectionLine(XMLoadFloat4(&mPlanes[3]), XMLoadFloat4(&mPlanes[0]));
    XMStoreFloat3(&mCorners[1], ComputeIntersection(XMLoadFloat4(&mPlanes[4]), ray));
    XMStoreFloat3(&mCorners[2], ComputeIntersection(XMLoadFloat4(&mPlanes[5]), ray));

    ray = ComputeIntersectionLine(XMLoadFloat4(&mPlanes[2]), XMLoadFloat4(&mPlanes[1]));
    XMStoreFloat3(&mCorners[4], ComputeIntersection(XMLoadFloat4(&mPlanes[4]), ray));
    XMStoreFloat3(&mCorners[7], ComputeIntersection(XMLoadFloat4(&mPlanes[5]), ray));

    ray = ComputeIntersectionLine(XMLoadFloat4(&mPlanes[1]), XMLoadFloat4(&mPlanes[3]));
    XMStoreFloat3(&mCorners[5], ComputeIntersection(XMLoadFloat4(&mPlanes[4]), ray));
    XMStoreFloat3(&mCorners[6],  ComputeIntersection(XMLoadFloat4(&mPlanes[5]), ray));
}

Ray Frustum::ComputeIntersectionLine(FXMVECTOR p1, FXMVECTOR p2)
{
    XMVECTOR direction = XMVector3Cross(p1, p2);
    XMVECTOR lengthSquared = XMVector3LengthSq(direction);
    XMVECTOR position = XMVector3Cross((-XMVectorGetW(p1) * p2) + (XMVectorGetW(p2) * p1), direction) / lengthSquared;

    return Ray(position, direction);
}

XMVECTOR Frustum::ComputeIntersection(FXMVECTOR& plane, Ray& ray)
{
    float value =  (-XMVectorGetW(plane) - XMVectorGetX(XMVector3Dot(plane, ray.PositionVector()))) / XMVectorGetX(XMVector3Dot(plane, ray.DirectionVector()));

    return (ray.PositionVector() + (ray.DirectionVector() * value));
}
}
