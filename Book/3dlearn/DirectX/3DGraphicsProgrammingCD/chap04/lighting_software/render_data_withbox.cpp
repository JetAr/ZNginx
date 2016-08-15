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

Vertex_VCN g_BoxVertices[g_iNumBoxVertices] =
{
	// bottom
	{
		Vector4(-0.5f, -0.5f,  0.5f), // v2
			Vector4( 1.0f,  1.0f,  1.0f),
			Vector4( 0.0f, -1.0f,  0.0f)
	},
	{
		Vector4(-0.5f, -0.5f, -0.5f), // v1
			Vector4( 1.0f,  1.0f,  1.0f),
			Vector4( 0.0f, -1.0f,  0.0f)
		},
		{
			Vector4( 0.5f, -0.5f, -0.5f), // v0
				Vector4( 1.0f,  1.0f,  1.0f),
				Vector4( 0.0f, -1.0f,  0.0f)
		},
		{
			Vector4( 0.5f, -0.5f,  0.5f), // v3
				Vector4( 1.0f,  1.0f,  1.0f),
				Vector4( 0.0f, -1.0f,  0.0f)
			},

			// top
			{
				Vector4(-0.5f,  0.5f,  0.5f), // v6
					Vector4( 1.0f,  1.0f,  1.0f),
					Vector4( 0.0f,  1.0f,  0.0f)
			},
			{
				Vector4( 0.5f,  0.5f,  0.5f), // v7
					Vector4( 1.0f,  1.0f,  1.0f),
					Vector4( 0.0f,  1.0f,  0.0f)
				},
				{
					Vector4( 0.5f,  0.5f, -0.5f), // v4
						Vector4( 1.0f,  1.0f,  1.0f),
						Vector4( 0.0f,  1.0f,  0.0f)
				},
				{
					Vector4(-0.5f,  0.5f, -0.5f), // v5
						Vector4( 1.0f,  1.0f,  1.0f),
						Vector4( 0.0f,  1.0f,  0.0f)
					},

					// left
					{
						Vector4(-0.5f, -0.5f, -0.5f), // v11
							Vector4( 1.0f,  1.0f,  1.0f),
							Vector4(-1.0f,  0.0f,  0.0f)
					},
					{
						Vector4(-0.5f, -0.5f,  0.5f), // v8
							Vector4( 1.0f,  1.0f,  1.0f),
							Vector4(-1.0f,  0.0f,  0.0f)
						},
						{
							Vector4(-0.5f,  0.5f,  0.5f), // v9
								Vector4( 1.0f,  1.0f,  1.0f),
								Vector4(-1.0f,  0.0f,  0.0f)
						},
						{
							Vector4(-0.5f,  0.5f, -0.5f), // v10
								Vector4( 1.0f,  1.0f,  1.0f),
								Vector4(-1.0f,  0.0f,  0.0f)
							},

							// right
							{
								Vector4( 0.5f, -0.5f, -0.5f), // v15
									Vector4( 1.0f,  1.0f,  1.0f),
									Vector4( 1.0f,  0.0f,  0.0f)
							},
							{
								Vector4( 0.5f,  0.5f, -0.5f), // v14
									Vector4( 1.0f,  1.0f,  1.0f),
									Vector4( 1.0f,  0.0f,  0.0f)
								},
								{
									Vector4( 0.5f,  0.5f,  0.5f), // v13
										Vector4( 1.0f,  1.0f,  1.0f),
										Vector4( 1.0f,  0.0f,  0.0f)
								},
								{
									Vector4( 0.5f, -0.5f,  0.5f), // v12
										Vector4( 1.0f,  1.0f,  1.0f),
										Vector4( 1.0f,  0.0f,  0.0f)
									},

									// front
									{
										Vector4(-0.5f, -0.5f,  0.5f), // v19
											Vector4( 1.0f,  1.0f,  1.0f),
											Vector4( 0.0f,  0.0f,  1.0f)
									},
									{
										Vector4( 0.5f, -0.5f,  0.5f), // v18
											Vector4( 1.0f,  1.0f,  1.0f),
											Vector4( 0.0f,  0.0f,  1.0f)
										},
										{
											Vector4( 0.5f,  0.5f,  0.5f), // v17
												Vector4( 1.0f,  1.0f,  1.0f),
												Vector4( 0.0f,  0.0f,  1.0f)
										},
										{
											Vector4(-0.5f,  0.5f,  0.5f), // v16
												Vector4( 1.0f,  1.0f,  1.0f),
												Vector4( 0.0f,  0.0f,  1.0f)
											},

											// back
											{
												Vector4(-0.5f, -0.5f, -0.5f), // v23
													Vector4( 1.0f,  1.0f,  1.0f),
													Vector4( 0.0f,  0.0f, -1.0f)
											},
											{
												Vector4(-0.5f,  0.5f, -0.5f), // v22
													Vector4( 1.0f,  1.0f,  1.0f),
													Vector4( 0.0f,  0.0f, -1.0f)
												},
												{
													Vector4( 0.5f,  0.5f, -0.5f), // v21
														Vector4( 1.0f,  1.0f,  1.0f),
														Vector4( 0.0f,  0.0f, -1.0f)
												},
												{
													Vector4( 0.5f, -0.5f, -0.5f), // v20
														Vector4( 1.0f,  1.0f,  1.0f),
														Vector4( 0.0f,  0.0f, -1.0f)
													},
};

unsigned short g_BoxIndices[g_iNumBoxIndices] =
{
	// bottom
	0,1,2,
	0,2,3,
	// top
	4,5,6,
	4,6,7,
	// left
	8,9,10,
	8,10,11,
	// right
	12,13,14,
	12,14,15,
	// front
	16,17,18,
	16,18,19,
	// back
	20,21,22,
	20,22,23
};

int g_iNumGridVertices = 0;
int g_iNumGridIndices = 0;
int g_iNumGridTriangles = 0;

Vertex_VCN *g_pGridVertices = NULL;
unsigned short *g_pGridIndices = NULL;

void *Allocate16BytesAlignedMemory(int size)
{
	char *block = new char[size + 16];
	int *aligned = (int*)((int)(block + 20) & 0xFFFFFFF0);
	aligned[-1] = (int)block;
	return aligned;
}

void Release16BytesAlignedMemory(void *memory)
{
	delete ((int**)memory)[-1];
}

bool GenerateGrids(int x_grids, int y_grids, Vertex_VCN **ppVertices, int *pNum_Vertices, unsigned short **ppIndices, int *pNum_Indices, int *num_triangles)
{
	const int triangles_per_row = x_grids * 2;
	const int indices_per_row = triangles_per_row + 2;

	*num_triangles = triangles_per_row * y_grids;

	int num_vertices = (x_grids + 1) * (y_grids + 1);
	*pNum_Vertices = num_vertices;
	Vertex_VCN *pVertices = (Vertex_VCN *) Allocate16BytesAlignedMemory(sizeof(Vertex_VCN)*num_vertices);
	*ppVertices = pVertices;
	if ( pVertices==NULL )
		return false;

	int num_indices = indices_per_row * y_grids;
	*pNum_Indices = num_indices;
	unsigned short *pIndices = new unsigned short[num_indices];
	*ppIndices = pIndices;
	if ( pIndices==NULL )
	{
		Release16BytesAlignedMemory(pVertices);
		return false;
	}

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
	int index_index = 0;
	vertex_index = 0;

	for ( y=0; y<y_grids; y++ )
	{
		if ( from_left_to_right )
		{
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

void CalculateDirectionalLight(Vertex_VCN *pVertices, int num_vertices, Vector4 &light_direction, Vector4 &light_color)
{
	for ( int i=0; i<num_vertices; i++ )
	{
		Vector4 normal = g_world_matrix.RotateVector(pVertices[i].m_Normal);
		Vector4 intensity = VectorDot(normal, light_direction);
		intensity.Abs();
		pVertices[i].m_Color = intensity * light_color;
	}
}

void CalculatePointLight(Vertex_VCN *pVertices, int num_vertices, Vector4 &light_position, Vector4 &light_color)
{
	for ( int i=0; i<num_vertices; i++ )
	{
		Vector4 position = pVertices[i].m_Position * g_world_matrix;
		Vector4 vertex_to_light = light_position - position; 
		vertex_to_light.Normalize();
		Vector4 normal = g_world_matrix.RotateVector(pVertices[i].m_Normal);
		Vector4 intensity = VectorDot(normal, vertex_to_light);
		intensity.Abs();
		pVertices[i].m_Color = intensity * light_color;
	}
}