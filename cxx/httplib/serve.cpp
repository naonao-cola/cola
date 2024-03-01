
#include "httplib.h"


void* pHandle = nullptr;


void test_http(void* ptr,const httplib::Request& req, httplib::Response& res) {

   auto files_size = req.files.size();
   if (req.has_file("algo_cfg") && req.has_file("img_data") && req.has_file("img_json")) {

        auto algo_cfg_1 = req.get_file_value("algo_cfg");
        auto img_json_1 = req.get_file_value("img_json");
        json algo_cfg = json::parse(algo_cfg_1.content);
        json image_info = json::parse(img_json_1.content);

        int w = image_info["img_w"];
        int h = image_info["img_h"];
        int c = image_info["img_c"];

        auto content = req.get_file_value("img_data").content;
      
        std::vector<char> data;
        std::copy(content.data(), content.data()+ content.size(), std::back_inserter(data));
        cv::Mat src =  cv::imdecode(cv::Mat(data), cv::IMREAD_COLOR);
        tapp_algo_config(ptr, DumpJson(algo_cfg));

        g_imgs.push_back(src);

        tapp_run(ptr, src.data, DumpJson(image_info));

        res.status = 200;
        res.set_content(g_ret, "text/plain");
    }
}

int  main(){
	
	
	httplib::Server svr;
	pHandle = tapp_init();
	tapp_common_config(pHandle, DumpJson(common_cfg));
	//tapp_algo_config(pHandle, DumpJson(algo_cfg));
	tapp_register_result_callback(pHandle, RecultCallback);
	
	svr.Post("/inference", [&](const httplib::Request& req, httplib::Response& res){ test_http(pHandle,req, res); });
	svr.listen("192.168.0.37", 8080);
	tapp_destroy(pHandle);

	return 0;
}