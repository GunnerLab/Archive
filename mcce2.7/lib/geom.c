#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "mcce.h"

float dvv(VECTOR v1, VECTOR v2)
{  float dx, dy, dz;
   dx = v2.x- v1.x;
   dy = v2.y- v1.y;
   dz = v2.z- v1.z;
   return sqrt(dx*dx+dy*dy+dz*dz);
}

float ddvv(VECTOR v1, VECTOR v2)
{  float dx, dy, dz;
   dx = v2.x- v1.x;
   dy = v2.y- v1.y;
   dz = v2.z- v1.z;
   return dx*dx+dy*dy+dz*dz;
}

VECTOR vector_vplusv(VECTOR v1, VECTOR v2)
{  VECTOR z;
   z.x = v1.x + v2.x;
   z.y = v1.y + v2.y;
   z.z = v1.z + v2.z;
   return z;
}

VECTOR vector_normalize(VECTOR v)
{  VECTOR vn;
   float d;
   d = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
   if (d < 1.0e-20) {
      vn.x = 0.0;
      vn.y = 0.0;
      vn.z = 0.0;
   }
   else {
      vn.x = v.x/d;
      vn.y = v.y/d;
      vn.z = v.z/d;
   }
   return vn;
}

VECTOR vector_vminusv(VECTOR v1, VECTOR v2)
{  VECTOR z;
   z.x = v1.x - v2.x;
   z.y = v1.y - v2.y;
   z.z = v1.z - v2.z;
   return z;
}

VECTOR vector_vxv(VECTOR v1, VECTOR v2)
{  VECTOR z;
   z.x = v1.y*v2.z - v1.z*v2.y;
   z.y = v1.z*v2.x - v1.x*v2.z;
   z.z = v1.x*v2.y - v1.y*v2.x;
   return z;
}

VECTOR vector_rescale(VECTOR v, float c) {
    VECTOR z;
    z.x = v.x * c;
    z.y = v.y * c;
    z.z = v.z * c;
    return z;
}

VECTOR vector_sum3v(VECTOR v1, VECTOR v2, VECTOR v3) {
    VECTOR z;
    z.x = v1.x + v2.x + v3.x;
    z.y = v1.y + v2.y + v3.y;
    z.z = v1.z + v2.z + v3.z;
    return z;
}

VECTOR vector_neg(VECTOR v) {
    VECTOR z;
    z.x = -v.x;
    z.y = -v.y;
    z.z = -v.z;
    return z;
}

float vdotv(VECTOR v1, VECTOR v2)
{  return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

void geom_apply(GEOM op, VECTOR *v)
{  float vh[4];

   vh[0] = v->x;
   vh[1] = v->y;
   vh[2] = v->z;
   vh[3] = 1.0;

   v->x = op.M[0][0]*vh[0] + op.M[0][1]*vh[1] + op.M[0][2]*vh[2] + op.M[0][3]*vh[3];
   v->y = op.M[1][0]*vh[0] + op.M[1][1]*vh[1] + op.M[1][2]*vh[2] + op.M[1][3]*vh[3];
   v->z = op.M[2][0]*vh[0] + op.M[2][1]*vh[1] + op.M[2][2]*vh[2] + op.M[2][3]*vh[3];

   return;
}



void geom_reset(GEOM *op)
{  op->M[0][0] = 1.0; op->M[0][1] = 0.0; op->M[0][2] = 0.0; op->M[0][3] = 0.0;
   op->M[1][0] = 0.0; op->M[1][1] = 1.0; op->M[1][2] = 0.0; op->M[1][3] = 0.0;
   op->M[2][0] = 0.0; op->M[2][1] = 0.0; op->M[2][2] = 1.0; op->M[2][3] = 0.0;
   op->M[3][0] = 0.0; op->M[3][1] = 0.0; op->M[3][2] = 0.0; op->M[3][3] = 1.0;
   return;
}

void geom_roll(GEOM *op, float phi, LINE axis)
{  /*--- "Rotate about an arbitrary axis"  CRC Math handbook 4th edition */
   VECTOR v;
   float t, SIN, COS;
   int i, j;
   float m1[4][4], m2[4][4], m3[4][4];

   /* extreme contidions */
   t = fabs(axis.t.x) + fabs(axis.t.y) + fabs(axis.t.z);
   if ((fabs(phi) < 1.0E-8) || t <1.0E-8) return;


   /* translate to (0,0,0) */
   m1[0][0] = 1.0; m1[0][1] = 0.0; m1[0][2] = 0.0; m1[0][3] = -axis.p0.x;
   m1[1][0] = 0.0; m1[1][1] = 1.0; m1[1][2] = 0.0; m1[1][3] = -axis.p0.y;
   m1[2][0] = 0.0; m1[2][1] = 0.0; m1[2][2] = 1.0; m1[2][3] = -axis.p0.z;
   m1[3][0] = 0.0; m1[3][1] = 0.0; m1[3][2] = 0.0; m1[3][3] = 1.0;
   mxm4(m1, op->M, m2);

   /* rotate */
   v = axis.t;

   SIN = sin(phi); COS = cos(phi);

   m1[0][0] = v.x*v.x*(1.0-COS) + COS;
   m1[0][1] = v.x*v.y*(1.0-COS) - v.z*SIN;
   m1[0][2] = v.x*v.z*(1.0-COS) + v.y*SIN;

   m1[1][0] = v.x*v.y*(1.0-COS) + v.z*SIN;
   m1[1][1] = v.y*v.y*(1.0-COS) + COS;
   m1[1][2] = v.y*v.z*(1.0-COS) - v.x*SIN;

   m1[2][0] = v.x*v.z*(1.0-COS) - v.y*SIN;
   m1[2][1] = v.y*v.z*(1.0-COS) + v.x*SIN;
   m1[2][2] = v.z*v.z*(1.0-COS) + COS;

   m1[0][3] = 0.0; m1[1][3] = 0.0; m1[2][3] = 0.0; m1[3][3] = 1.0;
   m1[3][0] = 0.0; m1[3][1] = 0.0; m1[3][2] = 0.0;

   mxm4(m1, m2, m3);

   /* translate back */
   m1[0][0] = 1.0; m1[0][1] = 0.0; m1[0][2] = 0.0; m1[0][3] = axis.p0.x;
   m1[1][0] = 0.0; m1[1][1] = 1.0; m1[1][2] = 0.0; m1[1][3] = axis.p0.y;
   m1[2][0] = 0.0; m1[2][1] = 0.0; m1[2][2] = 1.0; m1[2][3] = axis.p0.z;
   m1[3][0] = 0.0; m1[3][1] = 0.0; m1[3][2] = 0.0; m1[3][3] = 1.0;
   mxm4(m1, m3, m2);

   /* load the result in m2 */
   for (i=0; i<4; i++)
      for (j=0; j<4; j++)
         op->M[i][j] = m2[i][j];

   return;
}

void mxm4(float m1[4][4], float m2[4][4], float m3[4][4])
{  m3[0][0]=m1[0][0]*m2[0][0]+m1[0][1]*m2[1][0]+m1[0][2]*m2[2][0]+m1[0][3]*m2[3][0];
   m3[0][1]=m1[0][0]*m2[0][1]+m1[0][1]*m2[1][1]+m1[0][2]*m2[2][1]+m1[0][3]*m2[3][1];
   m3[0][2]=m1[0][0]*m2[0][2]+m1[0][1]*m2[1][2]+m1[0][2]*m2[2][2]+m1[0][3]*m2[3][2];
   m3[0][3]=m1[0][0]*m2[0][3]+m1[0][1]*m2[1][3]+m1[0][2]*m2[2][3]+m1[0][3]*m2[3][3];

   m3[1][0]=m1[1][0]*m2[0][0]+m1[1][1]*m2[1][0]+m1[1][2]*m2[2][0]+m1[1][3]*m2[3][0];
   m3[1][1]=m1[1][0]*m2[0][1]+m1[1][1]*m2[1][1]+m1[1][2]*m2[2][1]+m1[1][3]*m2[3][1];
   m3[1][2]=m1[1][0]*m2[0][2]+m1[1][1]*m2[1][2]+m1[1][2]*m2[2][2]+m1[1][3]*m2[3][2];
   m3[1][3]=m1[1][0]*m2[0][3]+m1[1][1]*m2[1][3]+m1[1][2]*m2[2][3]+m1[1][3]*m2[3][3];

   m3[2][0]=m1[2][0]*m2[0][0]+m1[2][1]*m2[1][0]+m1[2][2]*m2[2][0]+m1[2][3]*m2[3][0];
   m3[2][1]=m1[2][0]*m2[0][1]+m1[2][1]*m2[1][1]+m1[2][2]*m2[2][1]+m1[2][3]*m2[3][1];
   m3[2][2]=m1[2][0]*m2[0][2]+m1[2][1]*m2[1][2]+m1[2][2]*m2[2][2]+m1[2][3]*m2[3][2];
   m3[2][3]=m1[2][0]*m2[0][3]+m1[2][1]*m2[1][3]+m1[2][2]*m2[2][3]+m1[2][3]*m2[3][3];

   m3[3][0]=m1[3][0]*m2[0][0]+m1[3][1]*m2[1][0]+m1[3][2]*m2[2][0]+m1[3][3]*m2[3][0];
   m3[3][1]=m1[3][0]*m2[0][1]+m1[3][1]*m2[1][1]+m1[3][2]*m2[2][1]+m1[3][3]*m2[3][1];
   m3[3][2]=m1[3][0]*m2[0][2]+m1[3][1]*m2[1][2]+m1[3][2]*m2[2][2]+m1[3][3]*m2[3][2];
   m3[3][3]=m1[3][0]*m2[0][3]+m1[3][1]*m2[1][3]+m1[3][2]*m2[2][3]+m1[3][3]*m2[3][3];

   return;
}

void geom_move(GEOM *op, VECTOR v)
{  int i, j;
   float m1[4][4], m2[4][4];

   /* translation */
   m1[0][0] = 1.0; m1[0][1] = 0.0; m1[0][2] = 0.0; m1[0][3] = v.x;
   m1[1][0] = 0.0; m1[1][1] = 1.0; m1[1][2] = 0.0; m1[1][3] = v.y;
   m1[2][0] = 0.0; m1[2][1] = 0.0; m1[2][2] = 1.0; m1[2][3] = v.z;
   m1[3][0] = 0.0; m1[3][1] = 0.0; m1[3][2] = 0.0; m1[3][3] = 1.0;

   /* apply translation */
   mxm4(m1, op->M, m2);

   /* load the geom operation */
   for (i=0; i<4; i++)
      for (j=0; j<4; j++)
         op->M[i][j] = m2[i][j];

   return;
}

GEOM geom_3v_onto_3v(VECTOR v1, VECTOR v2, VECTOR v3, VECTOR t1, VECTOR t2, VECTOR t3)
{  GEOM op;
   float angle;
   LINE axis;
   VECTOR v123;
   PLANE plane_v123, plane_t123;

   /* step 1, superimpose v1 to t1 */
   geom_reset(&op);
   geom_move(&op, vector_vminusv(t1, v1));

   /* step 2, align v1-v2 to t1-t2 */
   angle = all(line_2v(v1, v2), line_2v(t1, t2));
   axis.p0 = t1;
   axis.t  = vector_normalize(vector_vxv(vector_vminusv(v2, v1), vector_vminusv(t2, t1)));
   geom_roll(&op, angle, axis);

   /* normal direction of plane v123 should be updated */
   geom_apply(op, &v1);
   geom_apply(op, &v2);
   geom_apply(op, &v3);
   plane_v123 = plane_3v(v1, v2, v3);
   plane_t123 = plane_3v(t1, t2, t3);
   v123 = plane_v123.t;

   /*step 3, align v1-v2-v3 to t1-t2-t3 */
   angle = avv(v123, plane_t123.t);
   axis.p0 = t1;
   axis.t  = vector_normalize(vector_vxv(v123, plane_t123.t));
   geom_roll(&op, angle, axis);

   return op;
}


void geom_inverse(GEOM *op)
{  float t[4][4];
   float s;		/* the determinant of this matrix */

   memcpy(t, op->M, 16*sizeof(float));

   s = det4(t);

   op->M[0][0] = (t[1][1] * (t[2][2] * t[3][3] - t[2][3] * t[3][2]) +
                  t[1][2] * (t[2][3] * t[3][1] - t[2][1] * t[3][3]) +
                  t[1][3] * (t[2][1] * t[3][2] - t[2][2] * t[3][1])) / s;

   op->M[0][1] = (t[2][1] * (t[0][2] * t[3][3] - t[0][3] * t[3][2]) +
                  t[2][2] * (t[0][3] * t[3][1] - t[0][1] * t[3][3]) +
                  t[2][3] * (t[0][1] * t[3][2] - t[0][2] * t[3][1])) / s;

   op->M[0][2] = (t[3][1] * (t[0][2] * t[1][3] - t[0][3] * t[1][2]) +
                  t[3][2] * (t[0][3] * t[1][1] - t[0][1] * t[1][3]) +
                  t[3][3] * (t[0][1] * t[1][2] - t[0][2] * t[1][1])) / s;

   op->M[0][3] = (t[0][1] * (t[1][3] * t[2][2] - t[1][2] * t[2][3]) +
                  t[0][2] * (t[1][1] * t[2][3] - t[1][3] * t[2][1]) +
                  t[0][3] * (t[1][2] * t[2][1] - t[1][1] * t[2][2])) / s;

   op->M[1][0] = (t[1][2] * (t[2][0] * t[3][3] - t[2][3] * t[3][0]) +
                  t[1][3] * (t[2][2] * t[3][0] - t[2][0] * t[3][2]) +
                  t[1][0] * (t[2][3] * t[3][2] - t[2][2] * t[3][3])) / s;

   op->M[1][1] = (t[2][2] * (t[0][0] * t[3][3] - t[0][3] * t[3][0]) +
                  t[2][3] * (t[0][2] * t[3][0] - t[0][0] * t[3][2]) +
                  t[2][0] * (t[0][3] * t[3][2] - t[0][2] * t[3][3])) / s;

   op->M[1][2] = (t[3][2] * (t[0][0] * t[1][3] - t[0][3] * t[1][0]) +
                  t[3][3] * (t[0][2] * t[1][0] - t[0][0] * t[1][2]) +
                  t[3][0] * (t[0][3] * t[1][2] - t[0][2] * t[1][3])) / s;

   op->M[1][3] = (t[0][2] * (t[1][3] * t[2][0] - t[1][0] * t[2][3]) +
                  t[0][3] * (t[1][0] * t[2][2] - t[1][2] * t[2][0]) +
                  t[0][0] * (t[1][2] * t[2][3] - t[1][3] * t[2][2])) / s;

   op->M[2][0] = (t[1][3] * (t[2][0] * t[3][1] - t[2][1] * t[3][0]) +
                  t[1][0] * (t[2][1] * t[3][3] - t[2][3] * t[3][1]) +
                  t[1][1] * (t[2][3] * t[3][0] - t[2][0] * t[3][3])) / s;

   op->M[2][1] = (t[2][3] * (t[0][0] * t[3][1] - t[0][1] * t[3][0]) +
                  t[2][0] * (t[0][1] * t[3][3] - t[0][3] * t[3][1]) +
                  t[2][1] * (t[0][3] * t[3][0] - t[0][0] * t[3][3])) / s;

   op->M[2][2] = (t[3][3] * (t[0][0] * t[1][1] - t[0][1] * t[1][0]) +
                  t[3][0] * (t[0][1] * t[1][3] - t[0][3] * t[1][1]) +
                  t[3][1] * (t[0][3] * t[1][0] - t[0][0] * t[1][3])) / s;

   op->M[2][3] = (t[0][3] * (t[1][1] * t[2][0] - t[1][0] * t[2][1]) +
                  t[0][0] * (t[1][3] * t[2][1] - t[1][1] * t[2][3]) +
                  t[0][1] * (t[1][0] * t[2][3] - t[1][3] * t[2][0])) / s;

   op->M[3][0] = (t[1][0] * (t[2][2] * t[3][1] - t[2][1] * t[3][2]) +
                  t[1][1] * (t[2][0] * t[3][2] - t[2][2] * t[3][0]) +
                  t[1][2] * (t[2][1] * t[3][0] - t[2][0] * t[3][1])) / s;

   op->M[3][1] = (t[2][0] * (t[0][2] * t[3][1] - t[0][1] * t[3][2]) +
                  t[2][1] * (t[0][0] * t[3][2] - t[0][2] * t[3][0]) +
                  t[2][2] * (t[0][1] * t[3][0] - t[0][0] * t[3][1])) / s;

   op->M[3][2] = (t[3][0] * (t[0][2] * t[1][1] - t[0][1] * t[1][2]) +
                  t[3][1] * (t[0][0] * t[1][2] - t[0][2] * t[1][0]) +
                  t[3][2] * (t[0][1] * t[1][0] - t[0][0] * t[1][1])) / s;

   op->M[3][3] = (t[0][0] * (t[1][1] * t[2][2] - t[1][2] * t[2][1]) +
                  t[0][1] * (t[1][2] * t[2][0] - t[1][0] * t[2][2]) +
                  t[0][2] * (t[1][0] * t[2][1] - t[1][1] * t[2][0])) / s;
   return;
}

LINE line_2v(VECTOR v1, VECTOR v2)
{  LINE line;
   line.p0 = v1;
   line.t = vector_normalize(vector_vminusv(v2, v1));
   return line;
}

PLANE plane_3v(VECTOR v1, VECTOR v2, VECTOR v3)
{  PLANE plane;
   plane.p0 = v1;
   plane.t = vector_normalize(vector_vxv(vector_vminusv(v2, v1),vector_vminusv(v3, v2)));
   return plane;
}

float det3(float m[][3])
{  return m[0][0]*m[1][1]*m[2][2]
        - m[0][0]*m[1][2]*m[2][1]
        + m[0][1]*m[1][2]*m[2][0]
        - m[0][1]*m[1][0]*m[2][2]
        + m[0][2]*m[1][0]*m[2][1]
        - m[0][2]*m[1][1]*m[2][0];
}

float det4(float m[][4])
{  return (m[0][0] * m[1][1] - m[0][1] * m[1][0]) * (m[2][2] * m[3][3] - m[2][3] * m[3][2])
        - (m[0][0] * m[1][2] - m[0][2] * m[1][0]) * (m[2][1] * m[3][3] - m[2][3] * m[3][1])
        + (m[0][0] * m[1][3] - m[0][3] * m[1][0]) * (m[2][1] * m[3][2] - m[2][2] * m[3][1])
        + (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * (m[2][0] * m[3][3] - m[2][3] * m[3][0])
        - (m[0][1] * m[1][3] - m[0][3] * m[1][1]) * (m[2][0] * m[3][2] - m[2][2] * m[3][0])
        + (m[0][2] * m[1][3] - m[0][3] * m[1][2]) * (m[2][0] * m[3][1] - m[2][1] * m[3][0]);
}

float avv(VECTOR v1, VECTOR v2)
{  float t;
   v1 = vector_normalize(v1); v2 = vector_normalize(v2);
   t = v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
   if (t>1.0) t = 1.0;
   else if (t < -1.0) t = -1.0;
   return acos(t);
}

float all(LINE line1, LINE line2)
{  return avv(line1.t, line2.t);
}

