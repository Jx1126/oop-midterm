#pragma once

struct TimeSystem
{
public:
    // Creating variables to store each part of the timestamp
    int year;
    int month;
    int day;
    int hour;
    int minute;
    double second;

    // Creating a constructor for the TimeSystem struct
    TimeSystem(int _year, int _month, int _day, int _hour, int _minute, double _second);

    bool operator<(const TimeSystem &other) const;
};