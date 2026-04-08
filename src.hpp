#ifndef SRC_HPP
#define SRC_HPP

#include <iostream>
#include <algorithm>
#include <string>
#include "base.hpp"

using namespace std;

struct date {
    int year, month, day;
    date() = default;
    date(int y, int m, int d) : year(y), month(m), day(d) {}
    
    bool operator<(const date& other) const {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        return day < other.day;
    }
    
    friend istream& operator>>(istream& is, date& d) {
        is >> d.year >> d.month >> d.day;
        return is;
    }
};

class mail : public object {
protected:
    string postmark;
    date send_date;
    date arrive_date;

public:
    mail() = default;

    mail(string _contain_, string _postmark_, date send_d, date arrive_d) : object(_contain_) {
        postmark = _postmark_;
        send_date = send_d;
        arrive_date = arrive_d;
    }

    string send_status(int y, int m, int d) override {
        return "not send";
    }

    string type() override {
        return "no type";
    }

    void print() override {
        object::print();
        cout << "[mail] postmark: " << postmark << endl;
    }

    void copy(object *o) override {
        if (this == o) return;
        contain = reinterpret_cast<mail *>(o)->contain;
        postmark = reinterpret_cast<mail *>(o)->postmark;
        send_date = reinterpret_cast<mail *>(o)->send_date;
        arrive_date = reinterpret_cast<mail *>(o)->arrive_date;
    }
    
    ~mail() override {}
};

class air_mail : public mail {
protected:
    string airlines;
    date take_off_date;
    date land_date;

public:
    air_mail() = default;

    air_mail(string _contain_, string _postmark_, date send_d, date arrive_d, date take_off, date land, string _airline) 
        : mail(_contain_, _postmark_, send_d, arrive_d) {
        take_off_date = take_off;
        land_date = land;
        airlines = _airline;
    }

    string send_status(int y, int m, int d) override {
        date ask_date(y, m, d);
        if (ask_date < send_date)
            return "mail not send";
        else if (ask_date < take_off_date)
            return "wait in airport";
        else if (ask_date < land_date)
            return "in flight";
        else if (ask_date < arrive_date)
            return "already land";
        else
            return "already arrive";
    }

    string type() override {
        return "air";
    }

    void print() override {
        mail::print();
        cout << "[air] airlines: " << airlines << endl;
    }

    void copy(object *o) override {
        if (this == o) return;
        mail::copy(o);
        take_off_date = reinterpret_cast<air_mail *>(o)->take_off_date;
        land_date = reinterpret_cast<air_mail *>(o)->land_date;
        airlines = reinterpret_cast<air_mail *>(o)->airlines;
    }
    
    ~air_mail() override {}
};

class train_mail : public mail {
protected:
    string *station_name;
    date *station_time;
    int len;

public:
    train_mail() : station_name(nullptr), station_time(nullptr), len(0) {}

    train_mail(string _contain_, string _postmark_, date send_d, date arrive_d, string *sname, date *stime, int station_num)
        : mail(_contain_, _postmark_, send_d, arrive_d) {
        len = station_num;
        if (len > 0) {
            station_name = new string[len];
            station_time = new date[len];
            for (int i = 0; i < len; ++i) {
                station_name[i] = sname[i];
                station_time[i] = stime[i];
            }
        } else {
            station_name = nullptr;
            station_time = nullptr;
        }
    }

    string send_status(int y, int m, int d) override {
        date ask_date(y, m, d);
        if (ask_date < send_date)
            return "mail not send";
        else if (len > 0 && ask_date < station_time[0])
            return "wait in station";
        else if (len > 0 && ask_date < station_time[len - 1]) {
            for (int i = 0; i < len - 1; ++i) {
                if (!(ask_date < station_time[i]) && ask_date < station_time[i + 1]) {
                    return "run between " + station_name[i] + " and " + station_name[i + 1];
                }
            }
            return ""; // Should not reach here
        }
        else if (ask_date < arrive_date)
            return "already land";
        else
            return "already arrive";
    }

    string type() override {
        return "train";
    }

    void print() override {
        mail::print();
        cout << "[train] station_num: " << len << endl;
    }

    void copy(object *o) override {
        if (this == o) return;
        mail::copy(o);
        train_mail *tm = reinterpret_cast<train_mail *>(o);
        int new_len = tm->len;
        string *new_sname = nullptr;
        date *new_stime = nullptr;
        if (new_len > 0) {
            new_sname = new string[new_len];
            new_stime = new date[new_len];
            for (int i = 0; i < new_len; ++i) {
                new_sname[i] = tm->station_name[i];
                new_stime[i] = tm->station_time[i];
            }
        }
        delete[] station_name;
        delete[] station_time;
        len = new_len;
        station_name = new_sname;
        station_time = new_stime;
    }

    ~train_mail() override {
        delete[] station_name;
        delete[] station_time;
    }
};

class car_mail : public mail {
protected:
    int total_mile;
    string driver;

public:
    car_mail() = default;

    car_mail(string _contain_, string _postmark_, date send_d, date arrive_d, int mile, string _driver)
        : mail(_contain_, _postmark_, send_d, arrive_d) {
        total_mile = mile;
        driver = _driver;
    }

    string send_status(int y, int m, int d) override {
        date ask_date(y, m, d);
        if (ask_date < send_date)
            return "mail not send";
        else if (ask_date < arrive_date) {
            auto get_days = [](const date& dt) {
                return dt.year * 360 + dt.month * 30 + dt.day;
            };
            int used_time = get_days(ask_date) - get_days(send_date);
            int total_time = get_days(arrive_date) - get_days(send_date);
            double current_mile = (double)used_time / (double)total_time * (double)total_mile;
            return to_string(current_mile);
        }
        else
            return "already arrive";
    }

    string type() override {
        return "car";
    }

    void print() override {
        mail::print();
        cout << "[car] driver_name: " << driver << endl;
    }

    void copy(object *o) override {
        if (this == o) return;
        mail::copy(o);
        total_mile = reinterpret_cast<car_mail *>(o)->total_mile;
        driver = reinterpret_cast<car_mail *>(o)->driver;
    }
    
    ~car_mail() override {}
};

void obj_swap(object *&x, object *&y) {
    object *temp = x;
    x = y;
    y = temp;
}

#endif
