#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "CSVReader.h"
#include "OrderBook.h"
#include "OrderBookEntry.h"
#include "Candlestick.h"

//START
// Defining the constructor for the Candlestick class
Candlestick::Candlestick(std::string _timestamp, double _open, double _high, double _low, double _close, std::string _market)
{
    timestamp = _timestamp;
    open = _open;
    high = _high;
    low = _low;
    close = _close;
    market = _market;
};

// Defining less than operator for TimeSystem struct
// Comparing the two TimeSystem objects
bool TimeSystem::operator<(const TimeSystem &alternative) const
{

    // Comparing their year, month, day, hour, minute and second
    if (year != alternative.year)
    {
        return year < alternative.year;
    }
    else if (month != alternative.month)
    {
        return month < alternative.month;
    }
    else if (day != alternative.day)
    {
        return day < alternative.day;
    }
    else if (hour != alternative.hour)
    {
        return hour < alternative.hour;
    }
    return minute / 30 < alternative.minute / 30;
};

// Grouping the OrderBookEntry objects by 30 minutes intervals because the data is too large
std::map<TimeSystem, std::vector<OrderBookEntry>> Candlestick::jointGroup(const std::string &csv_file, const std::string &product, OrderBookType type)
{
    // Reading the CSV file then storing the data in a vector
    std::vector<OrderBookEntry> entries = CSVReader::readCSV(csv_file);

    // Map to store the data in the form of TimeSystem and vector of OrderBookEntry
    std::map<TimeSystem, std::vector<OrderBookEntry>> joint_data;

    // Looping through all the entry in the vector
    for (const OrderBookEntry &entry : entries)
    {
        // Checking if the product and order type matches the input
        if (entry.product == product && entry.orderType == type)
        {
            // Extracting the TimeSystem object from the timestamp
            TimeSystem time_system = getTimeSystem(entry.timestamp);
            // Storing the data back into the map
            joint_data[time_system].push_back(entry);
        }
    }
    return joint_data;
};

TimeSystem Candlestick::getTimeSystem(const std::string &timestamp)
{

    // Tokenizing the timestamp string when encountering a space
    std::vector<std::string> timeframe_data = CSVReader::tokenise(timestamp, ' ');

    // Storing the tokenized date and time as two separate strings
    std::string date = timeframe_data[0];
    std::string time = timeframe_data[1];

    // Tokenizing the timeframe data when encountering a "/"
    std::vector<std::string> date_tokens = CSVReader::tokenise(date, '/');

    // Storing the year, month and day as integers
    int year = std::stoi(date_tokens[0]);
    int month = std::stoi(date_tokens[1]);
    int day = std::stoi(date_tokens[2]);

    // Tokenizing the timeframe data when encountering a ":"
    std::vector<std::string> time_tokens = CSVReader::tokenise(time, ':');

    // Storing the hour, minute and second as integers
    int hour = std::stoi(time_tokens[0]);
    int minute = std::stoi(time_tokens[1]);
    double second = std::stod(time_tokens[2]);

    // Returning the TimeSystem object
    TimeSystem a(year, month, day, hour, minute, second);

    return a;
};

// Editing the display time format
std::string Candlestick::editTime(int phase)
{

    // If the phase is less than 10 then add a 0 to make it 2 digits
    return (phase <= 9 ? "0" : "") + std::to_string(phase);
}

std::vector<Candlestick> Candlestick::combineData(std::map<TimeSystem, std::vector<OrderBookEntry>> joint_data)
{

    // Setting the boolean value to true because there is no previous data
    bool no_prev_data = true;

    double total;
    double open = 0.0;
    double close;
    std::string market;
    std::vector<Candlestick> combined_data;

    // Looping through all the data in the vector
    for (const auto &set : joint_data)
    {

        //
        TimeSystem date_time = set.first;
        const std::vector<OrderBookEntry> &entries = set.second;

        // If there is no previous data
        if (no_prev_data)
        {
            // Setting open as the first entry in the vector
            open = entries[0].price;
            // Setting the boolean to false because open has been set as the first entry
            no_prev_data = false;
        }
        else
        {
            // Setting open as the close of the previous timeframe
            open = close;
        }

        // Calculating the high and low
        // Declaring the high and low as open
        double high = open;
        double low = open;

        // Looping through the entries in the vector
        for (const auto &entry : entries)
        {

            // If the price is higher than the current high
            if (entry.price > high)
            {

                // Set the high as the price
                high = entry.price;
            }

            // If the price is lower than the current low
            if (entry.price < low)
            {
                // Set the low as the price
                low = entry.price;
            }
        }

        // Calculating the close
        // Declaring the total value and amount as 0
        double total_value = 0.0;
        double total_amount = 0.0;

        // Looping through the entries in the vector
        for (const auto &entry : entries)
        {

            // Calculating the total value and amount
            total_value += entry.price * entry.amount;
            total_amount += entry.amount;
        }

        // Calculating the close following the formula
        close = total_value / total_amount;

        // Checking if it's a bull or bear market
        // If the cloosing price is higher than the opening price
        if (close > open)
        {

            // Set the boolean to true because the price went up
            market = "Bullish";
        }
        if (close < open)
        {

            // Set the boolean to false because the price went down
            market = "Bearish";
        }

        // Calling editTime to fix the time display then combined as strings
        std::string timeframe = editTime(date_time.hour) + ":" + editTime(date_time.minute);

        // Creating a candlestick object
        Candlestick candlestick(timeframe, open, high, low, close, market);

        // Storing the candlestick object in the vector
        combined_data.push_back(candlestick);
    };

    return combined_data;
};

// Finding the highest price
double Candlestick::findMaxPrice(std::vector<Candlestick> &candlesticks)
{

    // Setting the the entry in the vector as the max price
    double max_price = candlesticks[0].high;

    // Looping through the entries in the vector
    for (const auto &candlestick : candlesticks)
    {

        // If the current price is higher than the max price
        if (candlestick.high > max_price)
        {

            // Set the current price as the max price
            max_price = candlestick.high;
        }
    }

    // Return the max price
    return max_price;
};

// Finding the lowest price
double Candlestick::findMinPrice(std::vector<Candlestick> &candlesticks)
{

    // Setting the the entry in the vector as the min price
    double min_price = candlesticks[0].low;

    // Looping through the entries in the vector
    for (const auto &candlestick : candlesticks)
    {

        // If the current price is lower than the min price
        if (candlestick.low < min_price)
        {

            // Set the current price as the min price
            min_price = candlestick.low;
        }
    }

    // Return the min price
    return min_price;
};

// Drawing the candlestick graph y axis
std::string Candlestick::generateY(std::ofstream &output_text, int y_axis, double y_min_price, double y_max_price, double canvas_y)
{

    // Setting the y tick label as an string
    std::string y_tick_label;

    // Padding for the y axis tick labels
    int y_ticks_padding = canvas_y / 5;

    // If the y axis is a multiple of the padding
    if (y_axis % y_ticks_padding == 0)
    {

        // Calculate the y tick value to display
        double y_ticks_value = y_max_price - y_axis * ((y_max_price - y_min_price) / canvas_y);

        // Set the y tick label and only display 6 characters
        y_tick_label = std::to_string(y_ticks_value).substr(0, 6);

        // Adding a space behind the label
        y_tick_label = y_tick_label + " ";
    }
    else
    {
        // Else increase the y tick label as 7 spaces
        y_tick_label = std::string(7, ' ');
    }

    return y_tick_label;
};

// Drawing the candlestick graph x axis
void Candlestick::generateX(std::ofstream &output_text, std::string y_tick_label, std::vector<Candlestick> candlesticks)
{

    // Draw the x axis line in the candlestick graph
    output_text << y_tick_label << "|" << std::string(16, '_') + std::string((19 * candlesticks.size() - 1) - 1, '_') + '>'<< std::endl;

    // Left padding for the fist timestamp in the x axis
    std::string x_tick_label = std::string(17, ' ');

    // Looping through the entries in the vector
    for (Candlestick &candlestick : candlesticks)
    {

        // Storing the timestamp as a string
        std::string parts = candlestick.timestamp;

        // A preset to adjust the left and right padding
        int x_ticks_pading = 11;

        // Left padding for the x tick labels
        int x_left_padding = (x_ticks_pading - parts.length()) / 2;

        // Right padding for the x tick labels
        int x_right_padding = x_ticks_pading - x_left_padding - parts.length();

        // Adding the paddings to the x tick labels
        parts = std::string(x_left_padding, ' ') + parts + std::string(x_right_padding, ' ');

        // Adding spaces between the x axis tick labels
        x_tick_label += parts;
        x_tick_label += std::string(9, ' ');
    }
    // Printing the x axis tick labels
    output_text << x_tick_label << std::endl;
}

// Setting up the candlestick
void Candlestick::drawCandlestickGraph(std::vector<Candlestick> candlesticks, std::string rename_text)
{

    // Canvas size for the y axis
    double canvas_y = 50;
    // Finding the min price and assigning it to y_min_price
    double y_min_price = findMinPrice(candlesticks);
    // Finding the max price and assigning it to y_max_price
    double y_max_price = findMaxPrice(candlesticks);

    // Allowing users to understand the graph is currently being generated
    std::cout << "Setting up candlestick graph..." << std::endl;
    // Printing the minimum price
    std::cout << "Min Price: " << y_min_price << std::endl;
    // Printining the maximum price
    std::cout << "Max Price: " << y_max_price << std::endl;

    // Creating a new file and renaming it
    std::ofstream output_text(rename_text);

    // Printing texts to the candlestick text file
    output_text << "The candlestick with '+' refers to a bullish market" << std::endl;
    output_text << "The candlestick with '-' refers to a bearish market" << std::endl;
    output_text << " " << std::endl;
    output_text << "The requested candlestick data :" << std::endl;
    output_text << " " << std::endl;

    // Looping through the y axis lines
    for (int y_axis = 0; y_axis <= canvas_y; ++y_axis)
    {

        // Calling generateY to draw the y axis
        std::string y_tick_label = generateY(output_text, y_axis, y_min_price, y_max_price, canvas_y);

        // If the y axis is less than the canvas size
        if (y_axis < canvas_y)
        {

            // Calling drawCandlestick to draw the candlestick graph
            drawCandlestick(output_text, y_axis, y_min_price, y_max_price, canvas_y, y_tick_label, candlesticks);
        }
        else
        {

            // Calling generateX to draw the x axis
            generateX(output_text, y_tick_label, candlesticks);
        }
    }
}

// Drawing the candlestick inside the graph
void Candlestick::drawCandlestick(std::ofstream &output_text, int y_axis, double y_min_price, double y_max_price, double canvas_y, std::string y_tick_label, std::vector<Candlestick> candlesticks)
{

    // Adding spaces between the candlesticks and the y axis
    std::string x_tick_label = y_tick_label + "|" + std::string(12, ' ');

    // Looping through the entries in the vector
    for (Candlestick &candlestick : candlesticks)
    {

        // Setting the parts as a string
        std::string parts;

        double higher, lower;
        // If the open is higher than the close
        if (candlestick.open > candlestick.close)
        {
            // Set the higher as the open
            higher = candlestick.open;
            // Set the lower as the close
            lower = candlestick.close;
        }
        else
        {
            // Else set the higher as the close
            higher = candlestick.close;
            // Else set the lower as the open
            lower = candlestick.open;
        }

        // Calculating the y tick value
        double y_ticks_value = y_max_price - y_axis * ((y_max_price - y_min_price) / canvas_y);
        // Calculating the y tick high and low
        double y_ticks_high = y_ticks_value + ((y_max_price - y_min_price) / canvas_y / 2);
        double y_ticks_low = y_ticks_value - ((y_max_price - y_min_price) / canvas_y / 2);

        // If the y tick value is between the high and low of the candlestick
        if (y_ticks_value >= candlestick.low && y_ticks_value <= candlestick.high)
        {
            // If the y tick is within the open and close of the candlestick
            // if (y_ticks_value >= lower && y_ticks_value <= higher)
            if(y_ticks_high >= lower && y_ticks_low <= higher)
            {
                // If the market is bullish
                if (candlestick.market == "Bullish")
                {
                    // Set the parts with + as a string to indicate the market is bullish
                    parts = "|+++++++|";
                }
                if (candlestick.market == "Bearish")
                {
                    // Else set the parts with - as a string to indicate the market is bearish
                    parts = "|-------|";
                }
            }
            else
            {
                // Else set the parts with | as a string
                parts = "    |    ";
            }
        }
        else
        {
            // Else set the parts to spaces
            parts = std::string(9, ' ');
        }

        // Adding the parts to candlesticks
        x_tick_label += parts;
        // Adding spaces between the candlesticks
        x_tick_label += std::string(10, ' ');
    }
    // Printing the candlesticks
    output_text << x_tick_label << std::endl;
}
//END