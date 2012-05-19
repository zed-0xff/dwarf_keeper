#include "fetcher.cpp"

class AsyncFetcher : Fetcher {

    CURLM* multi_handle;

    public:

    AsyncFetcher() : Fetcher(){
        multi_handle = curl_multi_init();
    }

    ~AsyncFetcher(){
        curl_multi_cleanup(multi_handle);
    }
};
