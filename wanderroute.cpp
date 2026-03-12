#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
using namespace std;

struct DPResult {
    double min_max_distance;
    std::vector<std::vector<double>> dp_table;
};

class Wanderroute {

    //initialized from input file in read_input_file()
    int destination_count;
    vector<int> distances = {1};
    int travel_days;
    string testfile_name = "input_file.txt";

    /* main functionality method of the program/class 
     * to be used to calculate the optimal travel distance per day
     */
    public: int calc_wander_route() {
        try {
            read_input_file();                                      // get user input
        }
        catch(invalid_argument& e) {                                // not a number in argument file
            cerr << e.what() << endl;
            cout << "wrong entry in arg_file. Exit program" << endl;
            return -1;
        }
        catch(exception e){
            cerr << e.what() << endl;
            cout << "Exit program" << endl;
            return -1;
        }
        int max_travel_distance = optimize_traveldistance();        // find the way greedy
        print_output_greedy(max_travel_distance);                   // create output as requested

        vector<int> dp_result = dp_test(distances, travel_days);    // find the way with DP
        print_output_dp(dp_result);                                 // create output as requested

        return 0;
    }
    

    private: void read_input_file() {
        bool destination_count_not_read = true;
        bool day_count_not_read = true;
        
        // Read from the text file
        ifstream file_reader(testfile_name);
        string line;
        int destination_index = 0;

        // Use a while loop together with the getline() function to read the file line by line
        while (getline(file_reader, line)) {
            if (stoi(line) < 0) {
                throw ("Negative entries not allowed.");
            }
            if (destination_count_not_read) {
                destination_count = stoi(line);
                destination_count_not_read = false;
                distances.resize(destination_count, 0);
            }
            else if (day_count_not_read) {
                travel_days = stoi(line);
                day_count_not_read = false;
                if (destination_count < travel_days) {
                    throw ("more days than destinations. Solution is trivial.");
                }
            }
            else {
                if (destination_index >= destination_count) {
                    cout << "Too many distances in file, exess entries will be ignored" << endl;
                    return;
                }
                distances[destination_index] = stoi(line);
                destination_index += 1;
            }
        }
        if (destination_index < destination_count) {
            cout << "Not enough distance entries. Will produce nullpointer." << endl;
            throw ("Not enough distance entries. Will produce nullpointer.");
        }
    }
    
    private: double calculate_distance(const vector<int>& distances, int start, int end) {
        if (start >= end || start < 0 || end > distances.size()) {
            return 0.0;
        }
        return accumulate(distances.begin() + start, distances.begin() + end, 0.0);
    }

    private: vector<int> dp_test(const vector<int>& dp_distances, int days) {
        const int n = dp_distances.size();
        const double INF = numeric_limits<double>::infinity();
        
        // init DP-table (d+1) × (n+1) (days x distances)
        vector<vector<double>> dp(days + 1, vector<double>(n + 1, INF));
        // init parent result table
        vector<vector<int>> parent(days + 1, vector<int>(n + 1, -1));
        
        // Basiswerte setzen
        dp[0][0] = 0.0;
        for (int d = 1; d <= days; ++d) {
            dp[d][0] = 0.0;
        }
        
        // doing the dp-algorithm
        for (int d = 1; d <= days; ++d) {
            for (int i = 1; i <= n; ++i) {
                for (int j = 0; j < i; ++j) {               // looking at each possible way j
                    if (dp[d - 1][j] == INF) continue;      // if the way until here is impossible, speed up the loop
                    double this_distance = max(dp[d - 1][j], calculate_distance(dp_distances, j, i));
                    if (this_distance < dp[d][i])
                    {
                        dp[d][i] = this_distance;
                        parent[d][i] = j;
                    }
                }
            }
        }
        // now create the backtracking result
        vector<int> path_result = {n};

        int current = n;
        for (int d = days; d > 0; d--) 
        {
            current = parent[d][current];
            path_result.push_back(current);
        }
        reverse(path_result.begin(), path_result.end());
        return path_result;
    }

    /* find the best travel distance per day
     */
    private: int optimize_traveldistance() {
        int min_distance = 0;
        int max_distance = 0;
        int i = 0;                                                          // calculate full distance
        while (i < destination_count) {
            max_distance += distances[i];
            i += 1;
        }
        
        while (min_distance < max_distance)
        {
            int mid_distance = (int)((min_distance + max_distance) / 2);    // start with middle

            if (check_distance_doable(mid_distance, travel_days)) {         // if the destination can be reached
                max_distance = mid_distance;                                // the distance will be shorter next try
            }
            else {                                                          // if destination was not reached
                min_distance = mid_distance + 1;                            // try again with a bigger minimum
            }
        }
        return min_distance;                                                // return the last minimum distance that worked
    }

    /* try if it is possible 
     * that the route can be handled 
     * in given travel days 
     * with this daily travel distance
     */
    private: bool check_distance_doable(int traveldistance_day, int traveldays) {
        
        int day = 0;
        int position = 0;
        int distance_travelled = distances[0];                      // init with first travel distance
        
        while (day < traveldays) {                                  // if we still have days left
            while (distance_travelled <= traveldistance_day) {      // and the max travel distance is not reached yet
                position += 1;                                      // go to the next destination
                if (position >= destination_count) {                // if last destination was reached, it is possible
                    return true;                                    // and also probably too long of a distance per day
                }
                distance_travelled += distances[position];          // add the next travel distance
            }
            day += 1;                                               // the distance for the day was reached, next day started
            distance_travelled = distances[position];               // init with the next possible distance and check again
        }
        if (position == destination_count) {                        //the last destination was reached for this daily travel distance
            cout << "Tage: " << day << endl;
            return true;
        }
        else return false;
    }

    int distance_check(int start, int end) 
    {
        int distance = 0;
        int i = start;
        while (i < end)
        {
            i += 1;
            distance += distances[i];
        }
        return distance;
    }

    /*nochmal ein neuer versuch, aber gleiches ergebnis: Nicht alle Tage werden genutzt bei großer datenmenge*/
    private: bool doable(int traveldistance_day, int traveldays)
    {
        int pos = 0;
        int day = 0;
        int distance = 0;
        while (pos < destination_count)
        {
            int next = pos+1;
            while (next < destination_count && distance_check(pos, next + 1) <= traveldistance_day)
            {
                next += 1;
            }
            if (distance_check(pos, next) > traveldistance_day) return false;
            pos = next;
            day +=1;
        }
        return day <= travel_days;
    }

    /*
     * Calculate the output required based on max travel distance
     */
    private: void print_output_greedy(int max_travel_distance) {
        cout << "Output for Greedy Algorithm: " << endl;
        cout << "" << endl;
        int distance = 0;                                                       // distance to print
        int day = 1;                                                            // day to print
        int i = 0;
        while (i < destination_count) {                                         // until last destination reached
            int next_distance = distance + distances[i];                        // calculate distance to next destination
            if (next_distance >= max_travel_distance) {                         // if distance to next destination bigger than max distance
                if (next_distance == max_travel_distance) {                     // if it is the exact distance, 
                    cout << day << ". Tag: " << next_distance << " km" << endl; // print the sum of distances
                    distance = 0;                                               // and set distance to 0 because we dont want to count that twice
                }
                else {
                    cout << day << ". Tag: " << distance << " km" << endl;      // print out the last destination
                    day += 1;                                                   
                    distance = distances[i];                                    // set distance to new target for next day
                }
            }
            else {                                                              // if max distance was not reached
                distance = next_distance;                                       // increase distance
            }
            i += 1;
        }
        if (distance < max_travel_distance) {                                   // the last print if not max distance on the last 
            cout << day << ". Tag: " << distance << " km" << endl;              // destination 
        }
        cout << "" << endl;
        cout << "Maximum: " << max_travel_distance << " km" << endl;
        cout << "" << endl;
    }

    private: void print_output_dp(vector<int> steps_result) {
        cout << "Output for Dynamic Programming Algorithm: " << endl;
        cout << "" << endl;
        int max_travel_distance = 0;                                                                                    // init max distance
        for (int i = 1; i < steps_result.size(); i++)                                                                   // step though the results
        {
            int this_distance = (int) calculate_distance(distances, steps_result[i - 1], steps_result[i]);              // calculate the distance form previous stop to this stop
            cout << i << ". Tag: " << this_distance << " km" << endl;
            if (max_travel_distance < this_distance) max_travel_distance = this_distance;                               // save the highes max distance
        }
        cout << "" << endl;
        cout << "Maximum: " << max_travel_distance << " km" << endl;
        cout << "" << endl;

        cout << "Press any key to end." << endl;
        getchar();
    }
};
    

int main() {
    Wanderroute route = Wanderroute();
    return route.calc_wander_route();
    cout << "Press any key to end." << endl;
    getchar();
    system("pause");
}

