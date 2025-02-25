#ifndef __GET_STOCKS_INFO_H__
#define __GET_STOCKS_INFO_H__
/*
新浪数据接口：http://hq.sinajs.cn/list={code}。{code}替换为股票代码，沪市股票代码加前缀sh，深市股票代码加前缀sz。
三、获取股票历史数据
    搜狐数据接口：http://q.stock.sohu.com/hisHq?code={code}&start={start}&end={end}，
    {code}替换为股票代码，大陆股票代码加前缀cn_。{start}替换为起始日期，{end}替换为结束日期。
    例如：在浏览器地址栏输入：http://q.stock.sohu.com/hisHq?code=cn_601766,cn_000002&start=20150601&end=20160414
    组中数据一次对应含义如下：
    日期,
    今日开盘价,
    今日收盘价,
    涨跌幅度，
    涨跌百分比,
    今日最低价,
    今日最高价，
    今日成交股票手数,
    今日成交金额(单位：万)

换手率

// 股票数据： https://blog.csdn.net/chongzhusong1067/article/details/101042772
// https://www.cnblogs.com/Walker-lyl/p/13874792.html
*/

#include "basic/byte_buffer.h"
#include "basic/wejson.h"
#include "basic/logger.h"
#include "reactor/reactor.h"
#include "util/util.h"
#include "protocol/protocol.h"

#include "stock_define.h"

using namespace basic;
using namespace reactor;
using namespace util;
using namespace os;
using namespace ptl;

class StockHistoryDataClient : public HttpNetClient {
public:
    StockHistoryDataClient(void);
    virtual ~StockHistoryDataClient(void);

    // 发送股票数据请求,参数：返回的股票数据，股票代码，股票天数数量，请求超时事件，返回值：0表示成功，-1表示失败
    int request_stock_data(basic::ByteBuffer &ret_data, std::string code, int data_num = 500, int timeout = 5000);
    // 处理接收的数据
    virtual int handle_msg(ptl::HttpPtl &http_ptl, ptl::HttpParse_ErrorCode err);

private:
    basic::ByteBuffer *buffer_ptr_;

    bool is_received_;     // 有没有收到请求回应
    File file_;
    std::string curr_stock_code_;
    WeJson all_stocks_;
};

///////////////////////////////////////////////////////////////////////////////////////////
class StockInfoFileManager {
public:
    StockInfoFileManager(void);
    ~StockInfoFileManager(void);

    int get_stock_codes_list(std::vector<std::string> &list);
    int request_all_stock_infos(void);

private:
    int stock_request_num_;
    StockHistoryDataClient client_;
};

///////////////////////////////////////////////////////////////////////////////////////////
class AnalysisData;
struct StockParam {
    AnalysisData *analysis;
    std::string code;
    ByteBuffer buffer;
    int start_day;
    double profit_ratio;
};

class AnalysisData {
public:
    AnalysisData(void);
    ~AnalysisData(void);

    // 加载所有股票数据
    static void* parse_data(void *arg);
    // 股票是否可以买入
    bool is_it_possible_to_buy(std::vector<double> datas, int start_days);
    // 股票是否可以买入 测试版
    bool is_it_possible_to_buy_Test(std::vector<double> datas, int start_days);
    // 计算股票某个时间段的平均值
    double avg_price(std::vector<double> &datas, int start_days, int days);
    // 计算选中的股票多久达到指定的盈率
    double arrive_specify_profit_ratio(std::vector<double> &datas, int start_days, double profit_ratio);
    // 打印数据
    void print_datas(int start_days, double profit_ratio);
    // 股票达到指定盈利的概率
    void probability_of_stock_reaching_specified_profit(void);
    // 清除数据
    void clear();

public:
    int stocks_num_;

public:
    Mutex lock_;                    // 多线程锁
    File test_output_file_;         // 测试数据输出文件
    File choosen_stocks_;           // 选中股票代码输出文件
    std::vector<std::string> choose_stocks_;    // 选中的股票
    std::vector<double> choose_stocks_prices_;  // 选中股票价格
    std::vector<int> arrive_specify_profit_ratio_days_; // 达到指定盈利点的天数
    std::map<std::string, double> new_prices_;  // 股票最新价格（取决于你选择的开始天数）
    std::map<double, int> probability_of_stock_reaching_specified_profit_; //每次测试时选中指定的股票达到指定盈利的概率
};
#endif