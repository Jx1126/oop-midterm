#pragma once

#include <map>
#include <vector>
#include "CSVReader.h"
#include "OrderBookEntry.h"
#include "TimeSystem.h"

class Candlestick
{
public:
    // Creating variables to store each part of the candlestick
    std::string timestamp;
    double open;
    double high;
    double low;
    double close;
    std::string market;

    // Creating a constructor for the Candlestick class
    Candlestick(std::string _timestamp, double _open, double _high, double _low, double _close, std::string _market);

    static std::map<TimeSystem, std::vector<OrderBookEntry>> jointGroup(const std::string &csv_file, const std::string &product, OrderBookType type);

    static std::vector<Candlestick> combineData(std::map<TimeSystem, std::vector<OrderBookEntry>> joint_data);

    static void drawCandlestickGraph(std::vector<Candlestick> candlesticks, std::string output_text);

private:
    // Creating a function to convert the timestamp into TimeSystem object
    static TimeSystem getTimeSystem(const std::string &timestamp);

    // Editing the time format shown
    static std::string editTime(int phase);

    // Finding the highest price in the vector
    static double findMaxPrice(std::vector<Candlestick> &candlesticks);

    // Finding the lowest price in the vector
    static double findMinPrice(std::vector<Candlestick> &candlesticks);

    // Generating the Y axis
    static std::string generateY(std::ofstream &output_text, int y_axis, double y_min_price, double y_max_price, double canvas_y);

    // Generating the X axis
    static void generateX(std::ofstream &output_text, std::string y_tick_label, std::vector<Candlestick> candlesticks);

    // Drawing the candlesticks
    static void drawCandlestick(std::ofstream &output_text, int y_axis, double y_min_price, double y_max_price, double canvas_y, std::string y_tick_label, std::vector<Candlestick> candlesticks);
};