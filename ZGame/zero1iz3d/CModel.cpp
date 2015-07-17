// CModel.cpp -


#include "main.h"
#include "CModel.h"

extern GLuint     texture[6];


////////////////////////////////////////////////////////////////////////////////////////////////
//// Name:
//// Desc:
////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: ComputeNormals()
//// Desc: izchisli normalite na modela
////////////////////////////////////////////////////////////////////////////////////////////////
void CModel::ComputeNormals()
{
    CVector vVector1, vVector2, vNormal, vPoly[3];

    // If there are no objects, we can skip this part
    if( numOfObjects <= 0)
        return;

    // Go through each of the objects to calculate their normals
    for(int index = 0; index < numOfObjects; index++)
    {
        // Get the current object
        t3DObject *pObj = &pObject[index];

        // Here we allocate all the memory we need to calculate the normals
        CVector *pNormals		= new CVector [pObj->numOfFaces];
        CVector *pTempNormals	= new CVector [pObj->numOfFaces];
        pObj->pNormals			= new CVector [pObj->numOfVerts];

        int i = 0;
        // Go though all of the faces of this object
        for(i=0; i < pObj->numOfFaces; i++)
        {
            // To cut down LARGE code, we extract the 3 points of this face
            vPoly[0] = pObj->pVerts[pObj->pFaces[i].vertIndex[0]];
            vPoly[1] = pObj->pVerts[pObj->pFaces[i].vertIndex[1]];
            vPoly[2] = pObj->pVerts[pObj->pFaces[i].vertIndex[2]];

            // Now let's calculate the face normals (Get 2 vectors and find the cross product of those 2)

            vVector1 = CVector( vPoly[0], vPoly[2] );   // Get the vector of the polygon (we just need 2 sides for the normal)
            vVector2 = CVector( vPoly[2], vPoly[1] );	// Get a second vector of the polygon

            //vNormal  = Cross(vVector1, vVector2);		// Return the cross product of the 2 vectors (normalize vector, but not a unit vector)
            vNormal.Product( vVector1, vVector2 );
            pTempNormals[i] = vNormal;					// Save the un-normalized normal for the vertex normals
            //vNormal  = Normalize(vNormal);				// Normalize the cross product to give us the polygons normal
            vNormal.Normalize();

            pNormals[i] = vNormal;						// Assign the normal to the list of normals
        }

        //////////////// Now Get The Vertex Normals /////////////////

        CVector vSum = CVector(0.0, 0.0, 0.0);
        CVector vZero = vSum;
        CVector vtmp;
        int shared=0;

        for (i = 0; i < pObj->numOfVerts; i++)		    	// Go through all of the vertices
        {
            for (int j = 0; j < pObj->numOfFaces; j++)	    // Go through all of the triangles
            {
                // Check if the vertex is shared by another face
                if (pObj->pFaces[j].vertIndex[0] == i ||
                        pObj->pFaces[j].vertIndex[1] == i ||
                        pObj->pFaces[j].vertIndex[2] == i)
                {
                    //vSum = AddVector(vSum, pTempNormals[j]);// Add the un-normalized normal of the shared face
                    vSum = vSum + pTempNormals[j];
                    shared++;								// Increase the number of shared triangles
                }
            }

            // Get the normal by dividing the sum by the shared.  We negate the shared so it has the normals pointing out.
            vtmp = vSum / float(-shared);
            pObj->pNormals[i] = vtmp;
            //pObject->pNormals[i] = DivideVectorByScaler(vSum, float(-shared));

            // Normalize the normal for the final vertex normal
            pObj->pNormals[i].Normalize(); // = Normalize(pObject->pNormals[i]);

            vSum = vZero;									// Reset the sum
            shared = 0;										// Reset the shared
        }

        // Free our memory and start over on the next object
        delete [] pTempNormals;
        delete [] pNormals;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: Render()
//// Desc: rendirai modela ( NE EFEKTIVNO, VAs needed ;)
////////////////////////////////////////////////////////////////////////////////////////////////
void CModel::Render()
{

    for ( int i = 0; i < numOfObjects; i++ )
    {
        // validen obekt ?
        if ( pObject.size() <= 0 ) break;

        t3DObject *pObj = &pObject[i];

        if ( pObj->bHasTexture )
        {
            glEnable( GL_TEXTURE_2D );
            //glColor3ub( 255, 255, 255 );
            glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

            // Bind the texture-at na ID-to na materiala ( ID setva se ot 3Dmax-a )
            int b = pObj->materialID;
            glBindTexture(GL_TEXTURE_2D, texture[pObj->materialID]);
            //glBindTexture(GL_TEXTURE_2D, texture[2]);
        }
        else
        {
            // turn off mapping
            glDisable( GL_TEXTURE_2D );
            glColor3ub( 255, 255, 255 );
        }

        // wireframe or normal mode...
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        /*glEnable(GL_COLOR_MATERIAL);
        float gray[] = { 0.5f, 0.5f, 0.5f };
        glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
        //glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gray );
        glColor3f( 0.0f, 1.0f, 0.0f );*/


        glBegin( GL_TRIANGLES );

        for ( int j = 0; j < pObj->numOfFaces; j++ )
        {
            // mini prez vseki vryh i go izrisuvai
            for ( int whichvertex = 0; whichvertex < 3; whichvertex++ )
            {
                // vzemi indexa za vsqka to4ka
                int index = pObj->pFaces[j].vertIndex[whichvertex];

                // dai normala na OpenGL za tazi tochka
                glNormal3f( pObj->pNormals[i].x, pObj->pNormals[i].y, pObj->pNormals[i].z );


                if ( pObj->bHasTexture )
                {
                    // v sluchai che ima textura -> U,V
                    if ( pObj->pTexVerts )
                        glTexCoord2f( pObj->pTexVerts[index].x, pObj->pTexVerts[index].y );
                }
                else
                {
                    if ( pMaterials.size() && pObj->materialID >= 0 )
                    {
                        // vzemi cvqt za obekta tyi kato e bez textura
                        BYTE *pColor = pMaterials[pObj->materialID].color;

                        glColor3ub( pColor[0], pColor[1], pColor[2] );
                    }
                }

                // podai tochkata
                glVertex3f( pObj->pVerts[index].x, pObj->pVerts[index].y, pObj->pVerts[index].z );

            }
        }
        glEnd();
    }

}


////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: Destroy()
//// Desc:
////////////////////////////////////////////////////////////////////////////////////////////////
void CModel::Destroy()
{

    for ( int i = 0; i < numOfObjects; i++ )
    {
        delete[] pObject[i].pTexVerts;
        delete[] pObject[i].pVerts;
        delete[] pObject[i].pNormals;
        delete[] pObject[i].pFaces;
    }
}





void CMesh::Destroy()
{

    // free mesh vertices
    if ( pvertices )
    {
        delete[] pvertices;
        pvertices = NULL;
    }

    // free faces
    if ( pfaces )
    {
        delete[] pfaces;
        pfaces = NULL;
    }

    // free normals
    if ( pnormals )
    {
        delete[] pnormals;
        pnormals = NULL;
    }

    // free texture vertices
    if ( ptexvertices )
    {
        delete[] ptexvertices;
        ptexvertices = NULL;
    }

    num_vertices = num_faces = num_texvertices = 0;

}
