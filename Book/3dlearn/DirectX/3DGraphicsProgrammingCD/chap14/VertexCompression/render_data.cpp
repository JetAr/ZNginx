#include "Vector4.h"
#include "render_data.h"
#include "gut.h"

Matrix4x4 g_sun_matrix, g_earth_matrix, g_moon_matrix;
// controller object
CGutUserControl g_Control;
// 球的模型
CompressedVertex *g_pCompressedVertices = NULL;
//
unsigned short *g_pSphereIndices = NULL;
//
int g_iNumSphereVertices = 0;
int g_iNumSphereTriangles = 0;
int g_iNumSphereIndices = 0;
//
bool CreateSphere(CompressedVertex **ppVertices, // 傳回球面的頂點
				  unsigned short **ppIndices, // 傳回球面的三角形索引
				  int stacks, // 緯度的切面數目
				  int slices // 徑度的切面數目
				  )
{
	*ppVertices = NULL;

	int num_vertices = (stacks+1)*(slices+1);
	int num_triangles = stacks*slices*2;

	CompressedVertex *pVertices = new CompressedVertex[num_vertices];
	if ( pVertices==NULL )
		return false;

	*ppVertices = pVertices;

	g_iNumSphereVertices = num_vertices;
	g_iNumSphereTriangles = num_triangles;
	g_iNumSphereIndices = num_triangles * 3;

	const float theta_start_degree = 0.0f;
	const float theta_end_degree = 360.0f;
	const float phi_start_degree = -90.0f;
	const float phi_end_degree = 90.0f;

	float ts = FastMath::DegreeToRadian(theta_start_degree);
	float te = FastMath::DegreeToRadian(theta_end_degree);
	float ps = FastMath::DegreeToRadian(phi_start_degree);
	float pe = FastMath::DegreeToRadian(phi_end_degree);

	float theta_total = te - ts;
	float phi_total = pe - ps;
	float theta_inc = theta_total/stacks;
	float phi_inc = phi_total/slices;

	Vector4 vRadius(255.0f, 255.0f, 255.0f);
	Vector4 vExpand(255.0f*0.5f);
	Vector4 vOne(1.0f);

	int i,j;
	int index = 0;
	float theta = ts;

	float sin_theta, cos_theta;
	float sin_phi, cos_phi;

	for ( i=0; i<=stacks; i++ )
	{
		float phi = ps;
		FastMath::SinCos(theta, sin_theta, cos_theta);

		for ( j=0; j<=slices; j++, index++ )
		{
			FastMath::SinCos(phi, sin_phi, cos_phi);

			Vector4 vNormal(cos_phi * cos_theta, sin_phi, cos_phi * sin_theta);
			Vector4 vPos = vNormal * vRadius;
			Vector4 vCompressedNormal = (vNormal + vOne) * vExpand;
			// vertex
			pVertices[index].m_Position[0] = (unsigned char) vCompressedNormal[0];
			pVertices[index].m_Position[1] = (unsigned char) vCompressedNormal[1];
			pVertices[index].m_Position[2] = (unsigned char) vCompressedNormal[2];
			pVertices[index].m_Position[3] = 255;
			// 球行的vertex跟normal在此剛好相同
			pVertices[index].m_Normal[0] = (unsigned char) vCompressedNormal[0];
			pVertices[index].m_Normal[1] = (unsigned char) vCompressedNormal[1];
			pVertices[index].m_Normal[2] = (unsigned char) vCompressedNormal[2];
			pVertices[index].m_Normal[3] = 255;
			// inc phi
			phi += phi_inc;
		}
		// inc theta
		theta += theta_inc;
	}

	int base = 0;
	index = 0;

	if ( ppIndices )
	{
		*ppIndices = NULL;
		unsigned short *pIndices = new unsigned short[num_triangles*3];
		if ( pIndices==NULL )
		{
			delete [] pVertices;
			return false;
		}

		*ppIndices = pIndices;

		// triangle list
		for ( i=0; i<stacks; i++ )
		{
			for ( j=0; j<slices; j++ )
			{
				pIndices[index++] = base;
				pIndices[index++] = base+1;
				pIndices[index++] = base+slices+1;

				pIndices[index++] = base+1;
				pIndices[index++] = base+slices+2;
				pIndices[index++] = base+slices+1;

				base++;
			}
			base++;
		}
	}

	return true;
}