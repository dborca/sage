/* reflection vector
 *  r = u - 2 * n * n~ * u
 *
 *      x       nx*nx  nx*ny  nx*nz   x
 *  r = y - 2 * ny*nx  ny*ny  ny*nz * y
 *      z       nz*nx  nz*ny  nz*nz   z
 *
 *      x       nx*nx*x + nx*ny*y + nx*nz*z
 *  r = y - 2 * ny*nx*x + ny*ny*y + ny*nz*z
 *      z       nz*nx*x + nz*ny*y + nz*nz*z
 *
 *      x - nx * 2.0F * (nx*x + ny*y + nz*z)
 *  r = y - ny * 2.0F * (nx*x + ny*y + nz*z)
 *      z - nz * 2.0F * (nx*x + ny*y + nz*z)
 */
void
TAG(tnl_reflect) (void)
{
    int i;

    for (i = 0; i < tnl_vb.len; i++) {
	GLfloat dp = 2.0F * DOT3(tnl_vb.neye[i], tnl_vb.veyn[i]);

	tnl_vb.refl[i][0] = tnl_vb.veyn[i][0] - tnl_vb.neye[i][0] * dp;
	tnl_vb.refl[i][1] = tnl_vb.veyn[i][1] - tnl_vb.neye[i][1] * dp;
	tnl_vb.refl[i][2] = tnl_vb.veyn[i][2] - tnl_vb.neye[i][2] * dp + 1.0F;
#if (IND & D_NEED_MVEC)
	tnl_vb.refl[i][3] = 0.5F / SQRT(SQLEN3(tnl_vb.refl[i]));
#endif
    }
}


#undef TAG
#undef IND
