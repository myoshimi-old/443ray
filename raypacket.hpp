#ifndef _INC_RAYPACKET
#define _INC_RAYPACKET

#include <vector>

class Raypacket{
public:
    vector<UINT8>    valid;
    vector<Vector3*> ray;
};

#endif // _INC_RAYPACKET
