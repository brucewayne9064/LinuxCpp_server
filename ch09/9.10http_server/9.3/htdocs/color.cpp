//code blocks
#include <iostream>
#include <string>
#include <cstdlib>
using namespace std;

//获取环境变量REQUEST_METHOD，即浏览器使用的方法
string get_request_method()
{
    char* method = getenv("REQUEST_METHOD");
    if (method == NULL) return "";
    else return string(method);
}

//获取环境变量CONTENT_LENGTH，即浏览器传递的参数长度
int get_content_length()
{
    char* len = getenv("CONTENT_LENGTH");
    if (len == NULL) return 0;
    else return atoi(len);
}

//获取环境变量QUERY_STRING，即浏览器传递的参数
string get_query_string()
{
    char* query = getenv("QUERY_STRING");
    if (query == NULL) return "";
    else return string(query);
}

//读取标准输入，获取参数内容
string get_stdin_string()
{
    int len = get_content_length();
    if (len == 0) return "";
    else
    {
        char* query = new char[len + 1];
        cin.read(query, len);
        query[len] = '\0';
        string result(query);
        delete[] query;
        return result;
    }
}

//解析参数，获取颜色值
string get_color(string query)
{
    size_t pos = query.find("color=");
    if (pos == string::npos) return "";
    else return query.substr(pos + 6);
}

//输出html内容，根据颜色值改变背景色
void output_html(string color)
{
    cout << "Content-type: text/html\r\n\r\n"; //输出http响应头
    cout << "<html>\n";
    cout << "<head>\n";
    cout << "<title>Color Test</title>\n";
    cout << "</head>\n";
    cout << "<body bgcolor=\"" << color << "\">\n"; //设置背景色
    cout << "<h1>Color Test</h1>\n";
    cout << "<p>The color is: " << color << "</p>\n";
    cout << "<form method=\"POST\" action=\"color.cgi\">\n"; //创建表单，提交颜色参数
    cout << "<input type=\"text\" name=\"color\" value=\"" << color << "\">\n";
    cout << "<input type=\"submit\" value=\"Change\">\n";
    cout << "</form>\n";
    cout << "</body>\n";
    cout << "</html>\n";
}

int main()
{
    string method = get_request_method(); //获取方法
    string query; //声明参数
    if (method == "GET") //如果是GET方法
        query = get_query_string(); //获取参数
    else if (method == "POST") //如果是POST方法
        query = get_stdin_string(); //获取参数
    else //如果是其他方法
        query = ""; //无参数
    string color = get_color(query); //获取颜色
    output_html(color); //输出html
    return 0;
}