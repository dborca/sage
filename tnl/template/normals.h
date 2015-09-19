/* The normals can be transformed in two ways:
 *
 * 1) [nx', ny', nz', q'] = [nx, ny, nz, q] * inv(M)
 *    if w = 0
 *       q = 0
 *    else
 *       q = -[nx, ny, nz] * t([x, y, z]) / w
 *
 * 2) [nx', ny', nz'] = [nx, ny, nz] * inv(up(M))
 *    where up(M) is upper 3x3 of matrix
 *
 * We are transforming with:
 *    [nx', ny', nz'] = [nx, ny, nz] * up(inv(M))
 */
static void
TAG(_tnl_calc_neye) (void)
{
    int i;
    GLbitfield mask;
    const GLfloat *imv = get_imv();
    GLfloat4 *n = NULL;
    GLfloat4 *normal = tnl_vb.attr[TNL_NORMAL].data;
    const GLuint normal_stride = tnl_vb.attr[TNL_NORMAL].stride;
#if (IND & NORM_RESCALE)
    const GLfloat f = 1.0F / SQRT(imv[8] * imv[8] + imv[9] * imv[9] + imv[10] * imv[10]);
#endif

    for (mask = TNL_NORMAL_BIT, i = 0; i < tnl_vb.len; i++, mask = tnl_vb.flags[i]) {
	if (mask & TNL_NORMAL_BIT) {
	    /* new normal, redo the transformation */
	    n = &tnl_vb.neye[i];
	    matrix_mul_vec_rot(n[0], imv, normal[0]);
#if (IND & NORM_RESCALE)
	    n[0][0] *= f;
	    n[0][1] *= f;
	    n[0][2] *= f;
#endif
#if (IND & NORM_NORMALIZE)
	    NORM3(n[0], +, n[0]);
#endif
	} else {
	    tnl_vb.neye[i][0] = n[0][0];
	    tnl_vb.neye[i][1] = n[0][1];
	    tnl_vb.neye[i][2] = n[0][2];
	}

	normal += normal_stride;
    }
}


static void
TAG(_tnl_init_neye) (void)
{
    tnl_calc_neye_tab[IND] = TAG(_tnl_calc_neye);
}


#undef TAG
#undef IND
