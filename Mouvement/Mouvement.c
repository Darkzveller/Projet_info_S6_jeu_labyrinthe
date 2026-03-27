#include "Mouvement.h"

void position_tresor(t_laby *laby, t_tuiles *tuiles)
{
    char *ptr = laby->labyData;
    int nord = 0;
    int est = 0;
    int sud = 0;
    int ouest = 0;
    int item = 0;
    int offset = 0;

    for (int y = 0; y < laby->sizeY; y++)
    {
        for (int x = 0; x < laby->sizeX; x++)
        {
            if (sscanf(ptr, "%d %d %d %d %d%n", &nord, &est, &sud, &ouest, &item, &offset) >= 5)
            {

                if (item > 1)
                {
                    tuiles->x[item] = x;
                    tuiles->y[item] = y;
                }

                ptr += offset;
            }
        }
    }
#if DEBUG_POS_TUILES
    for (int i = 0; i < NBR_TUILES; i++)
    {
        printf("item %d px : %d py %d", i, tuiles->x[i], tuiles->y[i]);
        printf("\n");
    }
#endif
}
