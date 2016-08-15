#include "Vector4.h"
#include "render_data.h"
#include "gut.h"

// 鏡頭位置
Vector4 g_eye(0.0f, 0.0f, 2.0f); 
// 鏡頭對準的點
Vector4 g_lookat(0.0f, 0.0f, 0.0f); 
// 鏡頭正上方的方向
Vector4 g_up(0.0f, 1.0f, 0.0f); 

GraphicsDevice_Simulation g_Simulation;

int g_iNumGridVertices = 0;
int g_iNumGridIndices = 0;
int g_iNumGridTriangles = 0;

Vertex_VCN *g_pGridVertices = NULL;
Vertex_DX9 *g_pGridVerticesDX9 = NULL;
unsigned short *g_pGridIndices = NULL;

// 產生x_grids * y_grids個格子的棋盤格模型
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
	// 使用triangle strip時, 三角形數目永遠等於索引值數目減去2
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
			// 在奇數列的時候, 三角形從左排到右
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
			// 在偶數列的時候, 三角形從右排到左
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
		delete [] *ppVertices;
		*ppVertices = NULL;
	}

	if ( *ppIndices )
	{
		delete [] *ppIndices;
		*ppIndices = NULL;
	}

	return true;
}

void GraphicsDevice_Simulation::SetViewMatrix(Matrix4x4 &matrix)
{
	m_ViewMatrix = matrix;
	m_WorldViewMatrix = m_WorldMatrix * m_ViewMatrix;
}

void GraphicsDevice_Simulation::SetWorldMatrix(Matrix4x4 &matrix)
{
	m_WorldMatrix = matrix;
	m_WorldViewMatrix = m_WorldMatrix * m_ViewMatrix;
}

void GraphicsDevice_Simulation::CalculateLighting(Vertex_VCN *pVertices, int num_vertices)
{
	// 記錄轉換後的光源資料
	Light_Info lights[g_iNumMaxLights];

	// 先把光源轉換到鏡頭座標系
	// 并把材質跟光源顏色做相乘, 可以省略重復的計算.
	for ( int l=0; l<g_iNumMaxLights; l++ )
	{
		if ( m_Lights[l].m_bEnabled )
		{
			lights[l] = m_Lights[l];

			lights[l].m_vPosition = m_Lights[l].m_vPosition * m_ViewMatrix;
			lights[l].m_vDirection = m_ViewMatrix.RotateVector(m_Lights[l].m_vDirection);
			if ( m_bAutoNormalize ) lights[l].m_vDirection.Normalize();
			lights[l].m_vAmbientColor =  m_vMaterialAmbient  * m_Lights[l].m_vAmbientColor;
			lights[l].m_vDiffuseColor =  m_vMaterialDiffuse  * m_Lights[l].m_vDiffuseColor;
			lights[l].m_vSpecularColor = m_vMaterialSpecular * m_Lights[l].m_vSpecularColor;
			float spotcutoff = FastMath::DegreeToRadian(m_Lights[l].m_fSpotlightCutoff * 0.5f);
			lights[l].m_fSpotlightHalfCutoffCosine = FastMath::Cos(spotcutoff);
		}
	}

	Vector4 vDefaultCameraDir(0.0f, 0.0f, 1.0f);

	// 計算基本的打光值
	Vector4 vGlobalAmbient = m_vAmbientLight * m_vMaterialAmbient;
	Vector4 vInitialLighting = m_vMaterialEmissive + vGlobalAmbient;

	for ( int i=0; i<num_vertices; i++ )
	{
		pVertices[i].m_Color = vInitialLighting;

		for ( int l=0; l<g_iNumMaxLights; l++ )
		{
			Light_Info *pLight = &lights[l];
			if ( !pLight->m_bEnabled )
				continue;

			Vector4 vPosition, vNormal;
			Vector4 vDiffuse, vSpecular;
			Vector4 vAttenuation;
			Vector4 vLightDir;
			Vector4 vCameraDir;

			// 把打光初值設定成這盞燈的環境光
			Vector4 vLighting = pLight->m_vAmbientColor;

			switch(pLight->m_eType)
			{
			case LIGHT_DIRECTIONAL:
				vNormal = m_WorldViewMatrix.RotateVector(pVertices[i].m_Normal);
				if ( m_bAutoNormalize ) vNormal.Normalize();

				{
					// diffuse lighting
					vDiffuse = Vector3Dot(vNormal, pLight->m_vDirection);
					vDiffuse.Clamp_to_0();
					vDiffuse *= pLight->m_vDiffuseColor;
				}
				{
					// specular lighting
					if ( m_bLocalViewer )
					{
						vPosition = pVertices[i].m_Position * m_WorldViewMatrix;
						vCameraDir = -vPosition;
						vCameraDir.Normalize();
					}
					else
					{
						vCameraDir = vDefaultCameraDir;
					}

					if ( m_bPhongSpecular )
					{
						// `比較精確的公式`
						Vector4 vProjectedLightDir = vNormal * Vector3Dot(pLight->m_vDirection, vNormal);
						Vector4 vReflectDir = 2.0f * vProjectedLightDir - vLightDir;
						vReflectDir.Normalize();
						Vector4 vSpecularCosine = Vector3Dot(vReflectDir, vCameraDir);
						vSpecularCosine.Clamp_to_0();
						float fSpecular = pow(vSpecularCosine.GetX(), m_fMaterialShininess);
						vSpecular = fSpecular * pLight->m_vSpecularColor;
					}
					else
					{
						// `Direct3D跟OpenGL使用的簡化公式`
						Vector4 vHalfDir = (pLight->m_vDirection + vCameraDir);
						vHalfDir.Normalize();
						Vector4 vSpecularCosine = Vector3Dot(vHalfDir, vNormal);
						vSpecularCosine.Clamp_to_0();
						float fSpecular = pow(vSpecularCosine.GetX(), m_fMaterialShininess);
						vSpecular = fSpecular * pLight->m_vSpecularColor;
					}
				}	
				vLighting += vDiffuse + vSpecular;
				break;

			case LIGHT_POINT:
				{
					vPosition = pVertices[i].m_Position * m_WorldViewMatrix;
					vNormal = m_WorldViewMatrix.RotateVector(pVertices[i].m_Normal);
					if ( m_bAutoNormalize ) vNormal.Normalize();
					vLightDir = pLight->m_vPosition - vPosition; 

					{
						// attenuation
						float light_distance = vLightDir.NormalizeAndGetLength();
						Vector4 vDistance(1.0f, light_distance, light_distance * light_distance);
						vAttenuation = Vector3Dot(vDistance, pLight->m_vAttenuation);
					}
					{
						// diffuse lighting
						vDiffuse = Vector3Dot(vNormal, vLightDir);
						vDiffuse.Clamp_to_0();
						vDiffuse *= pLight->m_vDiffuseColor;
					}
					{
						// specular lighting
						if ( m_bLocalViewer )
						{
							// `實際計算頂點到鏡頭的方向`
							vCameraDir = -vPosition;
							vCameraDir.Normalize();
						}
						else
						{
							// `假設頂點永遠在鏡頭的正前方`
							vCameraDir = vDefaultCameraDir;
						}

						if ( m_bPhongSpecular )
						{
							// `比較精確的公式`
							Vector4 vProjectedLightDir = vNormal * Vector3Dot(vLightDir, vNormal);
							Vector4 vReflectDir = 2.0f * vProjectedLightDir - vLightDir;
							vReflectDir.Normalize();
							Vector4 vSpecularCosine = Vector3Dot(vReflectDir, vCameraDir);
							vSpecularCosine.Clamp_to_0();
							float fSpecular = pow(vSpecularCosine.GetX(), m_fMaterialShininess);
							vSpecular = fSpecular * pLight->m_vSpecularColor;
						}
						else
						{
							// `Direct3D跟OpenGL使用的簡化公式`
							Vector4 vHalfDir = (vLightDir + vCameraDir);
							vHalfDir.Normalize();
							Vector4 vSpecularCosine = Vector3Dot(vHalfDir, vNormal);
							vSpecularCosine.Clamp_to_0();
							float fSpecular = pow(vSpecularCosine.GetX(), m_fMaterialShininess);
							vSpecular = fSpecular * pLight->m_vSpecularColor;
						}
					}
					vLighting += (vDiffuse + vSpecular) / vAttenuation;
				}
				break;

			case LIGHT_SPOT:
				{
					vPosition = pVertices[i].m_Position * m_WorldViewMatrix;
					vNormal = m_WorldViewMatrix.RotateVector(pVertices[i].m_Normal);
					if ( m_bAutoNormalize ) vNormal.Normalize();
					vLightDir = pLight->m_vPosition - vPosition; 
					float light_distance = vLightDir.NormalizeAndGetLength();
					Vector4 vCosine = Vector3Dot(pLight->m_vDirection, -vLightDir);
					vCosine.Clamp_to_0();
					float fCosine = vCosine.GetX();

					if ( fCosine >= pLight->m_fSpotlightHalfCutoffCosine )
					{
						{
							// attenuation
							Vector4 vDistance(1.0f, light_distance, light_distance * light_distance);
							vAttenuation = Vector3Dot(vDistance, pLight->m_vAttenuation);
						}
						{
							// diffuse
							vDiffuse = Vector3Dot(vNormal, vLightDir);
							vDiffuse.Clamp_to_0();
							vDiffuse *= pLight->m_vDiffuseColor;
						}
						{
							// specular lighting
							if ( m_bLocalViewer )
							{
								// `假設頂點永遠在鏡頭的正前方`
								vCameraDir = -vPosition;
								vCameraDir.Normalize();
							}
							else
							{
								// `假設頂點永遠在鏡頭的正前方`
								vCameraDir = vDefaultCameraDir;
							}

							if ( m_bPhongSpecular )
							{
								// `比較精確的公式`
								Vector4 vProjectedLightDir = vNormal * Vector3Dot(vLightDir, vNormal);
								Vector4 vReflectDir = 2.0f * vProjectedLightDir - vLightDir;
								vReflectDir.Normalize();
								Vector4 vSpecularCosine = Vector3Dot(vReflectDir, vCameraDir);
								vSpecularCosine.Clamp_to_0();
								float fSpecular = pow(vSpecularCosine.GetX(), m_fMaterialShininess);
								vSpecular = fSpecular * pLight->m_vSpecularColor;
							}
							else
							{
								Vector4 vHalfDir = (vLightDir + vCameraDir);
								vHalfDir.Normalize();
								Vector4 vSpecularCosine = Vector3Dot(vHalfDir, vNormal);
								vSpecularCosine.Clamp_to_0();
								float fSpecular = pow(vSpecularCosine.GetX(), m_fMaterialShininess);
								vSpecular = fSpecular * pLight->m_vSpecularColor;
							}
						}
						float fFalloff = pow(fCosine, pLight->m_fSpotlightExponent);
						vLighting += fFalloff * (vDiffuse + vSpecular) / vAttenuation;
					}
				}
				break;
			}
			pVertices[i].m_Color += vLighting;
			pVertices[i].m_Color.Saturate();
		}
	}
}

void ConvertToDX9(Vertex_VCN g_vertices_in[], Vertex_DX9 g_vertices_out[], int num_vertices)
{
	for ( int i=0; i<num_vertices; i++ )
	{
		g_vertices_in[i].m_Position.StoreXYZ(g_vertices_out[i].m_Position);
		GutConvertColor(g_vertices_in[i].m_Color, g_vertices_out[i].m_Color);
	}
}
