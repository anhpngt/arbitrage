#include "api.h"

API::API() : name_("API"), headers_(nullptr)
{
    // Setup connections
    curl_ = curl_easy_init();
    if (curl_)
    {
        headers_ = curl_slist_append(headers_, "content-Type: application/json");
        headers_ = curl_slist_append(headers_, "charsets: utf-8");

        curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers_);
        // curl_easy_setopt(curl_, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
        // curl_easy_setopt(curl_, CURLOPT_TIMEOUT, 10);           // time-out
        // curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, API::writeData);
        curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &http_data_);
    }
}

void API::release()
{
    curl_slist_free_all(headers_);
    curl_easy_cleanup(curl_);
}

API::~API()
{
    release();
}

bool API::requestPrice()
{
    http_data_.clear();
    res_ = curl_easy_perform(curl_);
    if (res_ == CURLE_OK)
        curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &http_code_);

    if (http_code_ == 200)
    {
        document_.Parse(http_data_.c_str());
        if(!document_.HasParseError())
        {
            return true;
        }
        else
        {
            cout << "ERROR: Could not parse HTTP data as JSON" << endl;
            // cout << "HTTP data was:\n" << http_data_ << endl;
            return false;
        }
    }
    else
    {
        cout << "ERROR: Could not get data from URL, HTTP code: " << http_code_ << endl;
        return false;
    }
}

std::size_t API::writeData(const char *in, std::size_t size, std::size_t num, std::string *out)
{
    const std::size_t totalBytes(size * num);
    out->append(in, totalBytes);
    return totalBytes;
}