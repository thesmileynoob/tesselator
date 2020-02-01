#include "object.h"

struct player : public object {

public:
    void Update() override;
    void Draw() override;
};
