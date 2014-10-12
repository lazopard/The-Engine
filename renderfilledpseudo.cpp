void TMesh::RenderFilled(PPC *ppc, FrameBuffer *fb, 
  unsigned int color, V3 L, float ka, int renderMode) {

  V3 *pverts = new V3[vertsN];
  for (int vi = 0; vi < vertsN; vi++) {
    //Project verts[vi] to pverts[vi]
  }

  for (int tri = 0; tri < trisN; tri++) {
    int vinds[3];
    vinds[0] = tris[tri*3+0];
    vinds[1] = tris[tri*3+1];
    vinds[2] = tris[tri*3+2];

    // Do not render triangle if any of its vertices had an invalid projection

    // Compute bounding box aabb of projected vertices

    // Clip aabb with frame

    // Setup edge equations ee0, ee1, ee2

    // Setup screen space linear variation of depth: zABC 
    // Setup screen space linear variation of red: redABC 
    // Setup screen space linear variation of green: greenABC 
    // Setup screen space linear variation of blue: blueABC 

    // for all rows v of aabb
        // for all columns u of row v
            
            // Current pixel is p(u, v)
            // Curretn pixel vector is pv(u+0.5, v+0.5, 1.0)

            // Check whether current pixel is inside triangle
            // if the pixel is on wrong side of any of the triangle edges
            // if (pv*ee0 < 0 || pv*ee1 || pv*ee2)
              // continue

            // Check whether triangle is closer than what was previously
                      // seen at this pixel
            // currz = zABC * pv
            // if currz < ZB[p]
                  // continue
            // ZB[p] = currz

            // pixel is insisde triangle and triangle is visible at pixel
            // compute color of pixel based on current triangle
            
            // if rendering mode is vertex color interpolation
            //      ssiRed = redABC * pv
            //      ssiGreen = greenABC * pv
            //      ssiBlue = blueABC * pv
            //      FB[p] = (ssiRed, ssiGreen, ssiB)

            // if rendering mode is per pixel diffuse lighting
            //      n.x = nxABC * pv
            //      n.y = nyABC * pv
            //      n.z = nzABC * pv
            //      normalize n
            //      surface point at current pixel P
            //      P = ppc->Unproject(u+0.5, v+0.5, currz)
            //      lv = (L-P).normalized()
            //      kd = lv * n; kd = (kd < 0) ? 0 : kd;
            //      FB[p] = color * (ka + (1-ka)*kd);
  }

  delete []pverts;
}

