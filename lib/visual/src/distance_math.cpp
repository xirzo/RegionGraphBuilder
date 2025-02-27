
#include <cmath>

long double rad(const long double& degree) {
    long double one_deg = (M_PI) / 180;
    return (one_deg * degree);
}

long double distance(long double lat1, long double long1, long double lat2,
                     long double long2) {
    lat1 = rad(lat1);
    long1 = rad(long1);
    lat2 = rad(lat2);
    long2 = rad(long2);

    long double dlong = long2 - long1;
    long double dlat = lat2 - lat1;

    long double ans =
        pow(sin(dlat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(dlong / 2), 2);

    ans = 2 * asin(sqrt(ans));

    constexpr long double R = 6371;

    ans = ans * R;

    return ans;
}
