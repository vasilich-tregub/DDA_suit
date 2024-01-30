// determine distances row - wise
for (int Y = 0; Y < height; ++Y)
{
    for (int X = 0; X < width; ++X)
    {
        rowdistance[X] = distance[Y * width + X];
        ++cyclecount;
    }
    for (int X = 0; X < width; ++X)
    {
        int distMin = rowdistance[X];
        for (int k = X + 1; k < width; ++k)
        {
            if (rowdistance[k] < (width * width + height * height))
            {
                int distHor = (k - X) * (k - X);
                if (distHor >= distMin)
                {
                    break;// pure horizontal component is longer then current distance
                }
                int dist = rowdistance[k] + distHor;
                if (distMin > dist)
                {
                    distMin = dist; // store new minimum distance
                }
            }
        }
        for (int k = X - 1; k > 0; --k) // compare to column positions backward
        {
            if (rowdistance[k] < (width * width + height * height))
                // combine vertical with horizontal component
            {
                int distHor = (k - X) * (k - X);
                if (distHor >= distMin)
                {
                    break;// pure horizontal component ...
                }
                int dist = rowdistance[k] + distHor;
                if (distMin > dist)
                {
                    distMin = dist;// store new minimum distance
                }
            }
        }
        distance[Y * width + X] = distMin; // assign minimum
    }
}
