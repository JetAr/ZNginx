#include "Vector4.h"
#include "render_data.h"
#include "gut.h"

// 鏡頭位置
Vector4 g_eye(0.0f, 0.0f, 2.0f); 
// 鏡頭對準的點
Vector4 g_lookat(0.0f, 0.0f, 0.0f); 
// 鏡頭正上方的方向
Vector4 g_up(0.0f, 1.0f, 0.0f); 
// 鏡頭轉換矩陣
Matrix4x4 g_view_matrix;
// 物件旋轉矩陣
Matrix4x4 g_world_matrix;

Vector4 g_vLightAmbient(0.0f, 0.0f, 0.0f, 0.0f);
Vector4 g_vLightDirection(0.0f, 0.0f, 1.0f);
Vector4 g_vLightPosition(0.0f, 0.0f, 0.5f);
Vector4 g_vLightColor(1.0f, 1.0f, 1.0f);
Vector4 g_vLightAttenuation(1.0f, 0.0f, 0.0f);
float	g_fSpotLightCutoff = 30.0f;
float	g_fSpotLightExponent = 1.0f;

int g_iNumGridVertices = 0;
int g_iNumGridIndices = 0;
int g_iNumGridTriangles = 0;
Vertex_VCN *g_pGridVertices = NULL;
Vertex_DX9 *g_pGridVerticesDX9 = NULL;
unsigned short *g_pGridIndices = NULL;

void ConvertToDX9(Vertex_VCN g_quad_in[], Vertex_DX9 g_quad_out[], int num_vertices)
{
	for ( int i=0; i<num_vertices; i++ )
	{
		g_quad_in[i].m_Position.StoreXYZ(g_quad_out[i].m_Position);
		GutConvertColor(g_quad_in[i].m_Color, g_quad_out[i].m_Color);
	}
}

// `產生x_grids * y_grids個格子的棋盤格模型`
bool GenerateGrids(int x_grids, int y_grids, Vertex_VCN **ppVertices, int &num_vertices, unsigned short **ppIndices, int &num_indices, int &num_triangles)
{
	const int triangles_per_row = x_grids * 2;
	const int indices_per_row = triangles_per_row + 2;

	num_vertices = (x_grids + 1) * (y_grids + 1);
	Vertex_VCN *pVertices = (Vertex_VCN *) GutAllocate16BytesAlignedMemory(sizeof(Vertex_VCN)*num_vertices);
	*ppVertices = pVertices;
	if ( pVertices==NULL )
		return false;

	num_indices = indices_per_row * y_grids;
	unsigned short *pIndices = new unsigned short[num_indices];
	*ppIndices = pIndices;
	if ( pIndices==NULL )
	{
		GutRelease16BytesAlignedMemory(pVertices);
		return false;
	}
	//` 使用triangle strip時, 三角形數目永遠等於索引值數目減去2`
	num_triangles = num_indices-2;

	Vector4 vCorner(-0.5f, 0.5f, 0.0f, 1.0f);
	Vector4 vStep(1.0f/float(x_grids), -1.0f/float(y_grids), 0.0f, 0.0f);
	Vector4 vPosition = vCorner;
	Vector4 vNormal(0.0f, 0.0f, 1.0f);
	Vector4 vColor(1.0f, 1.0f, 1.0f, 1.0f);

	int x,y;
	int vertex_index = 0;

	for ( y=0; y<=y_grids; y++)
	{
		vPosition[0] = vCorner[0];
		for ( x=0; x<=x_grids; x++, vertex_index++)
		{
			pVertices[vertex_index].m_Position = vPosition;
			pVertices[vertex_index].m_Normal = vNormal;
			pVertices[vertex_index].m_Color = vColor;
			vPosition[0] += vStep[0];
		}
		vPosition[1] += vStep[1];
	}

	const int vertices_per_row = x_grids + 1;
	bool from_left_to_right = true;
	int  index_index = 0;

	for ( y=0; y<y_grids; y++ )
	{
		if ( from_left_to_right )
		{
			// `在奇數列的時候, 三角形從左排到右.`
			pIndices[index_index++] = y * vertices_per_row;
			pIndices[index_index++] = y * vertices_per_row + vertices_per_row;

			for ( x=0; x<x_grids; x++ )
			{
				vertex_index = y * vertices_per_row + x;
				pIndices[index_index++] = vertex_index + 1;
				pIndices[index_index++] = vertex_index + 1 + vertices_per_row;
			}
		}
		else
		{
			// `在偶數列的時候, 三角形從右排到左.`
			pIndices[index_index++] = y * vertices_per_row + x_grids;
			pIndices[index_index++] = (y+1) * vertices_per_row + x_grids;

			for ( x=x_grids; x>0; x-- )
			{
				vertex_index = y * vertices_per_row + x;
				pIndices[index_index++] = vertex_index - 1;
				pIndices[index_index++] = vertex_index - 1 + vertices_per_row;
			}
		}
		from_left_to_right = !from_left_to_right;
	}

	return true;
}

bool ReleaseGridsResource(Vertex_VCN **ppVertices, unsigned short **ppIndices)
{
	if ( *ppVertices )
	{
		GutRelease16BytesAlignedMemory(*ppVertices);
		*ppVertices = NULL;
	}

	if ( *ppIndices )
	{
		delete [] *ppIndices;
		*ppIndices = NULL;
	}

	return true;
}

// `把顏色初值設定成環境光`
void CalculateAmbientLight(Vertex_VCN *pVertices, int num_vertices)
{
	for ( int i=0; i<num_vertices; i++ )
	{
		pVertices[i].m_Color = g_vLightAmbient;
	}
}

// `計算方向光, 它只跟頂點面向和光源方向有關.`
void CalculateDirectionalLight(Vertex_VCN *pVertices, int num_vertices)
{
	for ( int i=0; i<num_vertices; i++ )
	{
		// `求出轉換後在世界座標系的頂點面向, RotateVector函式只做旋轉, 忽略位移.`
		Vector4 normal = g_world_matrix.RotateVector(pVertices[i].m_Normal);
		// `頂點面向跟光線方向的交角, 決定反射光的強度.`
		Vector4 intensity = Vector3Dot(normal, g_vLightDirection);
		// `把intensity局限在永遠大於0的范圍`
		intensity.Clamp_to_0();
		// `累加上計算出來方向光的強度`
		pVertices[i].m_Color += intensity * g_vLightColor;
		pVertices[i].m_Color.Clamp_to_1();
	}
}

// `計算點光源, 它和頂點位置, 頂點面向, 光源位置有關.`
void CalculatePointLight(Vertex_VCN *pVertices, int num_vertices)
{
	for ( int i=0; i<num_vertices; i++ )
	{
		// `求出轉換後在世界座標系的頂點位置`
		Vector4 vPosition = pVertices[i].m_Position * g_world_matrix;
		// `求出轉換後在世界座標系的頂點面向, RotateVector函式只做旋轉, 忽略位移.`
		Vector4 vNormal = g_world_matrix.RotateVector(pVertices[i].m_Normal);
		// `計算出頂點位置到光源的方向跟長度`
		Vector4 vVertex_to_Light = g_vLightPosition - vPosition; 
		float light_distance = vVertex_to_Light.NormalizeAndGetLength();
		// `vDistance用來計算光線隨距離衰減公式中1/(a*1 + b*d + c*d^2)分母的(1, d, d^2)`
		Vector4 vDistance(1.0f, light_distance, light_distance * light_distance);
		// `g_vLightAttenuation里記錄了計算衰減公式1/(a + b*d + c*d^2)里的(a,b,c)`
		// `Vector3Dot(vDistance, g_vLightAttenuation) = (a,b,c) dot (1,d,d^2) = (a + b*d + c*d^2)`
		Vector4 vAttenuation = Vector3Dot(vDistance, g_vLightAttenuation);
		// `頂點面向跟光線方向的交角, 決定反射光的強度.`
		Vector4 vIntensity = Vector3Dot(vNormal, vVertex_to_Light);
		// `把intensity局限在永遠大於0的范圍`
		vIntensity.Clamp_to_0();
		// `累加上隨距離衰減的光線強度`
		pVertices[i].m_Color += vIntensity * g_vLightColor / vAttenuation;
		pVertices[i].m_Color.Clamp_to_1();
	}
}

// `計算點光源, 它和頂點位置, 頂點面向, 光源位置, 光源方向, 光柱交角有關.`
void CalculateSpotLight(Vertex_VCN *pVertices, int num_vertices)
{
	float fSpotLightCutoffCos = FastMath::Cos( FastMath::DegreeToRadian(g_fSpotLightCutoff) );

	for ( int i=0; i<num_vertices; i++ )
	{
		// `求出轉換後在世界座標系的頂點位置`
		Vector4 vPosition = pVertices[i].m_Position * g_world_matrix;
		// `求出轉換後在世界座標系的頂點面向, RotateVector函式只做旋轉, 忽略位移.`
		Vector4 vNormal = g_world_matrix.RotateVector(pVertices[i].m_Normal);
		// `計算出頂點位置到光源的方向跟長度`
		Vector4 vVertex_to_Light = g_vLightPosition - vPosition; 
		float light_distance = vVertex_to_Light.NormalizeAndGetLength();
		// `頂點面向跟光線方向的交角, 可以決定反射光的強度.`
		Vector4 vCosine = Vector3Dot(g_vLightDirection, vVertex_to_Light);
		// `把vCosine局限在永遠大於0的范圍`
		vCosine.Clamp_to_0();
		float fCosine = vCosine.GetX();
		if ( fCosine >= fSpotLightCutoffCos )
		{
			// `頂點跟光線的交角小於fSpotightCutoffCos時, 才落在光柱范圍內.`
			Vector4 vDistance(1.0f, light_distance, light_distance * light_distance);
			// `g_vLightAttenuation里記錄了計算衰減公式1/(a + b*d + c*d^2)里的(a,b,c)`
			// `Vector3Dot(vDistance, g_vLightAttenuation) = (a,b,c) dot (1,d,d^2) = (a + b*d + c*d^2)`
			Vector4 vAttenuation = Vector3Dot(vDistance, g_vLightAttenuation);
			// `比較靠近光柱外圍部分的頂點, 光線會衰減.`
			float fFalloff = pow(fCosine, g_fSpotLightExponent);
			Vector4 vIntensity = Vector3Dot(vNormal, vVertex_to_Light);
			pVertices[i].m_Color += fFalloff * vIntensity * g_vLightColor / vAttenuation;
			pVertices[i].m_Color.Clamp_to_1();
		}
	}
}
