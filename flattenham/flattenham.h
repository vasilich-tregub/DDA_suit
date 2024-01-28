#pragma once
/*
point[0] = (5,0)
point[1] = (6,4)
point[2] = (4,5)
point[3] = (1,5)
point[4] = (1,0)

Sum over the edges, (x2 - x1)(y2 + y1). If the result is positive the curve is clockwise, if it's negative the curve is counter-clockwise. (The result is twice the enclosed area, with a +/- convention.)

point[0] = (5,0)   edge[0]: (6-5)(4+0) =   4
point[1] = (6,4)   edge[1]: (4-6)(5+4) = -18
point[2] = (4,5)   edge[2]: (1-4)(5+5) = -30
point[3] = (1,5)   edge[3]: (1-1)(0+5) =   0
point[4] = (1,0)   edge[4]: (5-1)(0+0) =   0
                                         ---
                                         -44  counter-clockwise
*/