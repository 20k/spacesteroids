#include "asteroid_manager.hpp"

void asteroid_manager::discover(orbital* o, float year)
{
    if(id_map[o] != 0)
        return;

    const int min_year = 1950;

    id_map[o] = gid++;

    int my_id = id_map[o];

    int iyear = year - min_year;

    if(iyear < 0)
    {
        printf("warning iyear < 0 %i\n", iyear);

        iyear = 0;
    }

    ///if our year exceeds the current size of the discovery dates
    ///asteroid discovered in new year
    ///careful this doesnt explode memory over tens of thousands of years, but i guess
    ///its 24 * 8 bytes = 200 per year, so 10years = 1kb, 10k years = 1mb. Not really a problem then until 1 million years
    if(discovery_dates.size() <= iyear)
    {
        int cur_s = discovery_dates.size();

        discovery_dates.resize(iyear + 1);

        for(int i=cur_s; i<iyear+1; i++)
        {
            discovery_dates[i].resize(24);
        }
    }

    std::vector<std::vector<std::pair<float, int>>>& discovery_year = discovery_dates[iyear];

    float month_frac = year - (int)year;

    float half_month = month_frac * 24;

    int imonth = clamp(half_month, 0, 23);

    discovery_year[imonth].push_back({half_month - (int)half_month, my_id});

    //printf("hm %f\n", half_month);

    ///assume they're discovered sequentially?
    /*for(auto& i : discovery_year)
    {
        std::sort(i.begin(), i.end());
    }*/

    /*for(int half_month = 0; half_month < discovery_year.size(); half_month++)
    {

    }*/

    int icount = discovery_year[imonth].size()-1;

    int lc = icount % 25;
    int num = icount / 25;

    char first_letters[] = "abcdefghjklmnopqrstuvwxy";
    char second_letters[] = "acbdefghjklmnopqrstuvwxyz";

    char first_l = first_letters[imonth];
    char second_l = second_letters[lc];

    std::string name = std::to_string((int)year) + " " + first_l + second_l + std::to_string(num);

    cached_names[my_id] = name;
}

std::string asteroid_manager::get_name(orbital* o)
{
    //return cached_names[]

    if(id_map[o] == 0)
        return "";

    return cached_names[id_map[o]];

    //return "";
}
