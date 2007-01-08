struct OrderedPair {

    double x;
    double y;

    static bool xcomp(const OrderedPair& p1, const OrderedPair& p2) {
        return (p1.x < p2.x);
      }

    static bool ycomp(const OrderedPair& p1, const OrderedPair& p2) {
        return (p1.y < p2.y);
      }

};

