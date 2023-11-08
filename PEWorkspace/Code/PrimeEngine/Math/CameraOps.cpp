#ifndef __PYENGINE_2_0_CAMERA_OPS_H__
// These functions abstract low level camera matrix creation

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "CameraOps.h"

#if APIABSTRACTION_D3D11

#endif
/*
#if APIABSTRACTION_D3D9 | (APIABSTRACTION_OGLPC)
#include <d3d9.h>
#endif
*/
namespace CameraOps
{
	Matrix4x4 CreateViewMatrix(Vector3 &pos, Vector3 &target, Vector3 &up)
	{				
		Matrix4x4 res2;

		{

			double eyex = pos.m_x;
			double eyey = pos.m_y;
			double eyez = pos.m_z;
			double centerx = target.m_x;
			double centery = target.m_y;
			double centerz = target.m_z;
			double upx = up.m_x;
			double upy = up.m_y;
			double upz = up.m_z;

			double x[3], y[3], z[3], mag;

			/* Difference eye and center vectors to make Z vector. */
			z[0] = eyex - centerx;
			z[1] = eyey - centery;
			z[2] = eyez - centerz;
			/* Normalize Z. */
			mag = sqrt(z[0]*z[0] + z[1]*z[1] + z[2]*z[2]);
			if (mag) {
				z[0] /= mag;
				z[1] /= mag;
				z[2] /= mag;
			}

			/* Up vector makes Y vector. */
			y[0] = upx;
			y[1] = upy;
			y[2] = upz;

			/* X vector = Y cross Z. */
			x[0] = -( y[1]*z[2] - y[2]*z[1] );
			x[1] = -( -y[0]*z[2] + y[2]*z[0] );
			x[2] = -( y[0]*z[1] - y[1]*z[0] );

			/* Recompute Y = Z cross X. */
			y[0] = -(  z[1]*x[2] - z[2]*x[1] );
			y[1] = -( -z[0]*x[2] + z[2]*x[0] );
			y[2] = -(  z[0]*x[1] - z[1]*x[0] );

			/* Normalize X. */
			mag = sqrt(x[0]*x[0] + x[1]*x[1] + x[2]*x[2]);
			if (mag) {
				x[0] /= mag;
				x[1] /= mag;
				x[2] /= mag;
			}

			/* Normalize Y. */
			mag = sqrt(y[0]*y[0] + y[1]*y[1] + y[2]*y[2]);
			if (mag) {
				y[0] /= mag;
				y[1] /= mag;
				y[2] /= mag;
			}

			/* Build resulting view matrix. */
			res2.m[0][0] = float(x[0]);  res2.m[0][1] = float(x[1]);
			res2.m[0][2] = float(x[2]);  res2.m[0][3] = float(-x[0]*eyex + -x[1]*eyey + -x[2]*eyez);

			res2.m[1][0] = float(y[0]);  res2.m[1][1] = float(y[1]);
			res2.m[1][2] = float(y[2]);  res2.m[1][3] = float(-y[0]*eyex + -y[1]*eyey + -y[2]*eyez);

			res2.m[2][0] = float(-z[0]);  res2.m[2][1] = float(-z[1]);
			res2.m[2][2] = float(-z[2]); res2. m[2][3] = float(-(-z[0]*eyex + -z[1]*eyey + -z[2]*eyez));

			res2.m[3][0] = 0.0f;   res2.m[3][1] = 0.0f;  res2.m[3][2] = 0.0f;  res2.m[3][3] = 1.0f;
		}
		/*
		// code to change camera if coordinate system is flipped. should never use this but good for reference
		#if APIABSTRACTION_D3D9_PC
			res2.m[0][3] *= -1.0f;
			res2.m[1][3] *= -1.0f;
		
			res2.m[1][0] *= -1.0f;
			res2.m[1][1] *= -1.0f;
			res2.m[1][2] *= -1.0f;

			res2.m[0][0] *= -1.0f;
			res2.m[0][1] *= -1.0f;
			res2.m[0][2] *= -1.0f;
		#endif
		*/
		
		return res2;
	}

	Matrix4x4 CreateProjectionMatrix(PrimitiveTypes::Float32 verticalFov, PrimitiveTypes::Float32 aspectRatio,
		PrimitiveTypes::Float32 nearClip, PrimitiveTypes::Float32 farClip)
	{
		Matrix4x4 res2;

		//static void buildPerspectiveMatrix(double fieldOfView,
		//	double aspectRatio,
		//	double zNear, double zFar,
		//	float m[16])
		{
			double dAspectRatio = aspectRatio;
			double fieldOfView = verticalFov * aspectRatio;
			double zNear = nearClip;
			double zFar = farClip;

			
			double sine, cotangent, deltaZ;
			double radians = fieldOfView * 2.0/* / 4.0 * PrimitiveTypes::Constants::c_Pi_F64 / 180.0 */;

			deltaZ = zFar - zNear;
			sine = sin(radians);
			/* Should be non-zero to avoid division by zero. */
			assert(deltaZ);
			assert(sine);
			assert(dAspectRatio);
			cotangent = cos(radians) / sine;

			res2.m[0][0] = float(cotangent / dAspectRatio);
			res2.m[0][1] = 0.0f;
			res2.m[0][2] = 0.0f;
			res2.m[0][3] = 0.0f;

			res2.m[1][0] = 0.0f;
			res2.m[1][1] = float(cotangent);
			res2.m[1][2] = 0.0f;
			res2.m[1][3] = 0.0f;

			res2.m[2][0] = 0.0f;
			res2.m[2][1] = 0.0f;
			res2.m[2][2] = /* - */ float((zFar + zNear) / deltaZ);
			res2.m[2][3] = float( -1 /* * 2 */ * zNear * zFar / deltaZ );

			res2.m[3][0] = 0.0f;
			res2.m[3][1] = 0.0f;
			res2.m[3][2] = /* - */ 1.0f;
			res2.m[3][3] = 0.0f;
		}

		return res2;
	}

}; // namespace CameraOps

#endif
