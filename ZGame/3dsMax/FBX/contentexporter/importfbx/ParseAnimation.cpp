//-------------------------------------------------------------------------------------
// ParseAnimation.cpp
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//  
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//
// http://go.microsoft.com/fwlink/?LinkId=226208
//-------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "ParseAnimation.h"

using namespace ATG;
using namespace DirectX;

extern ATG::ExportScene* g_pScene;

struct AnimationScanNode
{
    INT iParentIndex;
    FbxNode* pNode;
    ExportAnimationTrack* pTrack;
    DWORD dwFlags;
    XMFLOAT4X4 matGlobal;
};

typedef std::vector<AnimationScanNode> ScanList;

//z 扫描一遍树节点，保存了父子关系等。
void ParseNode( FbxNode* pNode, ScanList& scanlist, DWORD dwFlags, INT iParentIndex, bool bIncludeNode )
{
    INT iCurrentIndex = iParentIndex;

    if( !bIncludeNode )
    {
        const CHAR* strNodeName = pNode->GetName();
        if( _stricmp( strNodeName, g_pScene->Settings().strAnimationRootNodeName ) == 0 )
        {
            bIncludeNode = true;
        }
    }

    if( bIncludeNode )
    {
        iCurrentIndex = static_cast<INT>( scanlist.size() );

        // add node to anim list
        AnimationScanNode asn = { 0 };
        asn.iParentIndex = iParentIndex;
        asn.pNode = pNode;
        asn.dwFlags = dwFlags;
        scanlist.push_back( asn );
    }

    DWORD dwChildCount = pNode->GetChildCount();
    for( DWORD i = 0; i < dwChildCount; ++i )
    {
        ParseNode( pNode->GetChild( i ), scanlist, dwFlags, iCurrentIndex, bIncludeNode );
    }
}

static XMFLOAT4X4 ConvertMatrix( const FbxMatrix& matrix )
{
    XMFLOAT4X4 matResult;
    auto fData = reinterpret_cast<float*>( &matResult );
    auto pSrcData = reinterpret_cast<const DOUBLE*>( &matrix );
    for( DWORD i = 0; i < 16; ++i )
    {
        fData[i] = (float)pSrcData[i];
    }
    return matResult;
}


void AddKey( AnimationScanNode& asn, const AnimationScanNode* pParent, FbxAMatrix& matFBXGlobal, float fTime )
{
    XMFLOAT4X4 matGlobal = ConvertMatrix( matFBXGlobal );
    asn.matGlobal = matGlobal;
    XMFLOAT4X4 matLocal = matGlobal;
    if( pParent )
    {
        //z 载入 父节点 的 matGlobal
        XMMATRIX m = XMLoadFloat4x4( &pParent->matGlobal );
        //z 得到一个逆矩阵
        XMMATRIX matInvParentGlobal = XMMatrixInverse( nullptr, m );

        m = XMLoadFloat4x4( &matGlobal );
        m = XMMatrixMultiply( m, matInvParentGlobal );

        XMStoreFloat4x4( &matLocal, m );
    }

    XMMATRIX matLocalFinal;
    g_pScene->GetDCCTransformer()->TransformMatrix( reinterpret_cast<XMFLOAT4X4*>( &matLocalFinal ), &matLocal );

    XMVECTOR vScale;
    XMVECTOR qRotation;
    XMVECTOR vTranslation;
    XMMatrixDecompose( &vScale, &qRotation, &vTranslation, matLocalFinal );

    XMFLOAT3 scale;
    XMStoreFloat3( &scale, vScale );

    XMFLOAT4 rot;
    XMStoreFloat4( &rot, qRotation );

    XMFLOAT3 trans;
    XMStoreFloat3( &trans, vTranslation );

    asn.pTrack->TransformTrack.AddKey( fTime, trans, rot, scale );
}

void CaptureAnimation( ScanList& scanlist, ExportAnimation* pAnim, FbxScene* pFbxScene )
{
    const float fDeltaTime = pAnim->fSourceSamplingInterval;
    const float fStartTime = pAnim->fStartTime;
    const float fEndTime = pAnim->fEndTime;
    float fCurrentTime = fStartTime;

    size_t dwNodeCount = scanlist.size();

    ExportLog::LogMsg( 2, "Capturing animation data from %Iu nodes, from time %0.3f to %0.3f, at an interval of %0.3f seconds.", dwNodeCount, fStartTime, fEndTime, fDeltaTime );

    while( fCurrentTime <= fEndTime )
    {
        FbxTime CurrentTime;
        CurrentTime.SetSecondDouble( fCurrentTime );
        for( size_t i = 0; i < dwNodeCount; ++i )
        {
            AnimationScanNode& asn = scanlist[i];

#if (FBXSDK_VERSION_MAJOR > 2014 || ((FBXSDK_VERSION_MAJOR==2014) && (FBXSDK_VERSION_MINOR>1) ) )
            auto pAnimEvaluator = pFbxScene->GetAnimationEvaluator();
#else
            auto pAnimEvaluator = pFbxScene->GetEvaluator();
#endif

            auto matGlobal = pAnimEvaluator->GetNodeGlobalTransform( asn.pNode, CurrentTime );
            AnimationScanNode* pParent = nullptr;
            if( asn.iParentIndex >= 0 )
                pParent = &scanlist[asn.iParentIndex];
            AddKey( asn, pParent, matGlobal, fCurrentTime - fStartTime );
        }
        fCurrentTime += fDeltaTime;
    }
}

//z strAnimStackName 待解析 anim stack 名称
void ParseAnimStack( FbxScene* pFbxScene, FbxString* strAnimStackName )
{
    // TODO - Ignore "Default"? FBXSDK_TAKENODE_DEFAULT_NAME
	//z 得到 找到对应的 anim stack ，查找类型为 FbxAnimStack 
    auto curAnimStack = pFbxScene->FindMember<FbxAnimStack>( strAnimStackName->Buffer() );
    if ( !curAnimStack )
        return;

	//z 处理不同版本 sdk 调用，如果 FBX SDK 版本 为 2014 并且 minor > 1 
#if (FBXSDK_VERSION_MAJOR > 2014 || ((FBXSDK_VERSION_MAJOR==2014) && (FBXSDK_VERSION_MINOR>1) ) )
    //z reset evaluator
	pFbxScene->GetAnimationEvaluator()->Reset();
#else
    pFbxScene->GetEvaluator()->SetContext( curAnimStack );
#endif

	//z 得到 take info ； Get information about an animation stack
    auto pTakeInfo = pFbxScene->GetTakeInfo( *strAnimStackName  );

    ExportLog::LogMsg( 2, "Parsing animation \"%s\"", strAnimStackName->Buffer() );

    auto pAnim = new ExportAnimation();
	//z 设置为 anim stack name
    pAnim->SetName( strAnimStackName->Buffer() );
	//? 暂时不清楚这个 dcobject 是做啥用的， pTakeInfo 是属于 fbx 的数据。
    pAnim->SetDCCObject( pTakeInfo );
    g_pScene->AddAnimation( pAnim );

    FbxTime FrameTime;
	//z void SetTime(int pHour, int pMinute, int pSecond, int pFrame=0, int pField=0, EMode pTimeMode=eDefaultMode);
	//z 根据 global settings 得到 time mode ；
    FrameTime.SetTime( 0, 0, 0, 1, 0, pFbxScene->GetGlobalSettings().GetTimeMode() );

	//z 根据模式和帧，得到每帧时间？FrameTime 已经设置为 1 帧，然后得到其所占用的秒的时间。
    float fFrameTime = (float)FrameTime.GetSecondDouble();
	//z 每帧 anim sample 的数目。得到每个 sample 的时间。默认30 frames/s，每一帧有一个 sample。
    float fSampleTime = fFrameTime / (float)g_pScene->Settings().iAnimSampleCountPerFrame;
    assert( fSampleTime > 0 );

    float fStartTime, fEndTime;
	if( pTakeInfo )
    {
		//z mLocalTimeSpan : Local time span, set to animation interval if it is left at the default value.
		//z 得到时间周期。0s--1s
        fStartTime = (float)pTakeInfo->mLocalTimeSpan.GetStart().GetSecondDouble();
        fEndTime = (float)pTakeInfo->mLocalTimeSpan.GetStop().GetSecondDouble();
    }
    else
    {
		//z 没有 takeinfo ，采用默认设置。
        FbxTimeSpan tlTimeSpan;
        pFbxScene->GetGlobalSettings().GetTimelineDefaultTimeSpan( tlTimeSpan );

        fStartTime = (float)tlTimeSpan.GetStart().GetSecondDouble();
        fEndTime = (float)tlTimeSpan.GetStop().GetSecondDouble();

        ExportLog::LogWarning( "Animation take \"%s\" has no takeinfo; using defaults.", pAnim->GetName().SafeString() );
    }

	//z 设置起讫时间
    pAnim->fStartTime = fStartTime;
    pAnim->fEndTime = fEndTime;
	//z 每个 frame 多少时间
    pAnim->fSourceFrameInterval = fFrameTime;
	//z 每个 anim sample 多长时间；一个 frame 里面可能渲染多个 sample ？
    pAnim->fSourceSamplingInterval = fSampleTime;

	//z 是否包含了所有的节点；如果 strAnimationRootNodeName 为空，那么不包含所有 nodes 。
    bool bIncludeAllNodes = true;
    if( strlen( g_pScene->Settings().strAnimationRootNodeName ) > 0 )
    {
        bIncludeAllNodes = false;
    }

	//z 扫描节点
    ScanList scanlist;
	//z void ParseNode(FbxNode* pNode, ScanList& scanlist, DWORD dwFlags, INT iParentIndex, bool bIncludeNode)
	//z 解析节点，
    ParseNode( pFbxScene->GetRootNode(), scanlist, 0, -1, bIncludeAllNodes );

    size_t dwTrackCount = scanlist.size();
    for( size_t i = 0; i < dwTrackCount; ++i )
    {
        const CHAR* strTrackName = scanlist[i].pNode->GetName();
        ExportLog::LogMsg( 4, "Track: %s", strTrackName );
        auto pTrack = new ExportAnimationTrack();
        pTrack->SetName( strTrackName );
        pTrack->TransformTrack.pSourceFrame = g_pScene->FindFrameByDCCObject( scanlist[i].pNode );
        pAnim->AddTrack( pTrack );
        scanlist[i].pTrack = pTrack;
    }

    CaptureAnimation( scanlist, pAnim, pFbxScene );

    pAnim->Optimize();
}

void ParseAnimation( FbxScene* pFbxScene )
{
    assert( pFbxScene != nullptr );

    // set animation quality settings
	//z 设置精度？公差
    ExportAnimation::SetAnimationExportQuality( g_pScene->Settings().iAnimPositionExportQuality, g_pScene->Settings().iAnimOrientationExportQuality, 50 );

    FbxArray<FbxString*> AnimStackNameArray;
	//z 这里的 stack 相当于组
	//z 得到所有 anim stack name 。填充 anim stack name array
    pFbxScene->FillAnimStackNameArray( AnimStackNameArray );

	//z 得到其长度
    DWORD dwAnimStackCount = static_cast<DWORD>( AnimStackNameArray.GetCount() );
    for( DWORD i = 0; i < dwAnimStackCount; ++i )
    {
		//z 解析所有的 anim stack
        ParseAnimStack( pFbxScene, AnimStackNameArray.GetAt(i) );
    }
}
