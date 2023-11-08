#ifndef HLSL_STANDARDINSTANCINGCONSTANTS
#define HLSL_STANDARDINSTANCINGCONSTANTS

#include "APIAbstraction.gpu"

#if APIABSTRACTION_D3D11

struct PerObjectInstanceData
{
	//matrix WVP;
	float4x3 W;
};

cbuffer cbInstanceControlConstants : register(b5)
{
	int4 gInstanceControl_xInstanceOffset_yzw;
};


struct BoneTQ
{
	float4 m_quat;
	float4 m_translation;
};

StructuredBuffer<Matrix> gInstancedBones : register(t64); // if cpu computes instanced palettes, they are put here
StructuredBuffer<BoneTQ> gAnimationData : register(t66);

// Compue Shader Map() ///////

#if PE_STORE_CS_MAP_RESULT_AS_MATRIX
	// in case of Compute Reduce, we will store data as Matrix so that we dont have to reconvert it
	RWStructuredBuffer<Matrix> gCSMapComputeTargetUAV : register(u0);
	StructuredBuffer<Matrix> gCSMapComputeTargetView : register(t67);  // used by second pass of compute shader
#else
	RWStructuredBuffer<BoneTQ> gCSMapComputeTargetUAV : register(u0);
	StructuredBuffer<BoneTQ> gCSMapComputeTargetView : register(t67);  // used by second pass of compute shader
#endif

// Compute Shader Reduce()

struct JointParentData
{
	int m_parentJointIndex;
};

StructuredBuffer<JointParentData> gSkeletonStructureView : register(t68);
StructuredBuffer<Matrix> gBoneInverses : register(t69);
StructuredBuffer<PerObjectInstanceData> gPerInstanceDataBuffer : register(t70);

RWStructuredBuffer<Matrix> gCSReduceComputeTargetUAV : register(u0); // we can reuse same u0 as above since we know one of these will be bound as UAV, not both at at time
StructuredBuffer<Matrix> gCSReduceComputeTargetView : register(t67); // used by VS pass after computing is done. We can resue t register from above since we know that only one will be bound at a time

#else // elif APIABSTRACTION_D3D9 | OGL | PS3

#endif

#endif // file guard
