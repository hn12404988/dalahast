#include <iostream>
#include <string.h>
#include <fcgio.h>
#include <review/client_core.h>
#include <review/control_filter.h>
#include <review/control_index.h>
#include <review/configure.h>

using namespace std;

// Maximum bytes
const unsigned long STDIN_MAX = 1000000;

/**
 * Note this is not thread safe due to the static allocation of the
 * content_buffer.
 */
string get_request_content(const FCGX_Request & request) {
    char * content_length_str = FCGX_GetParam("CONTENT_LENGTH", request.envp);
    unsigned long content_length = STDIN_MAX;

    if (content_length_str) {
        content_length = strtol(content_length_str, &content_length_str, 10);
        if (*content_length_str) {
            cerr << "Can't Parse 'CONTENT_LENGTH='"
                 << FCGX_GetParam("CONTENT_LENGTH", request.envp)
                 << "'. Consuming stdin up to " << STDIN_MAX << endl;
        }

        if (content_length > STDIN_MAX) {
            content_length = STDIN_MAX;
        }
    } else {
        // Do not read from stdin if CONTENT_LENGTH is missing
        content_length = 0;
    }

    char * content_buffer = new char[content_length];
    cin.read(content_buffer, content_length);
    content_length = cin.gcount();

    // Chew up any remaining stdin - this shouldn't be necessary
    // but is because mod_fastcgi doesn't handle it correctly.

    // ignore() doesn't set the eof bit in some versions of glibc++
    // so use gcount() instead of eof()...
    do cin.ignore(1024); while (cin.gcount() == 1024);

    string content(content_buffer, content_length);
    delete [] content_buffer;
    return content;
}

int main(void) {

    // Backup the stdio streambufs
    streambuf * cin_streambuf  = cin.rdbuf();
    streambuf * cout_streambuf = cout.rdbuf();
    streambuf * cerr_streambuf = cerr.rdbuf();
    FCGX_Request request;

    FCGX_Init();
    FCGX_InitRequest(&request, 0, 0);

	//const char * uri = FCGX_GetParam("REQUEST_URI", request.envp);
	string content;
	char callback_index;
	bool json;
	short int socket_index;
	client_core client;
	control_filter filter;
	control_index index;
	int max {0};
	max = filter.location.size();
	client.import_location(&filter.location,10);
	client.set_error_node(0,__FILE__);
    while (FCGX_Accept_r(&request) == 0) {
        fcgi_streambuf cin_fcgi_streambuf(request.in);
        fcgi_streambuf cout_fcgi_streambuf(request.out);
        fcgi_streambuf cerr_fcgi_streambuf(request.err);

        cin.rdbuf(&cin_fcgi_streambuf);
        cout.rdbuf(&cout_fcgi_streambuf);
        cerr.rdbuf(&cerr_fcgi_streambuf);
		
		// Note: the fcgi_streambuf destructor will auto flush
		cout << "Content-type: text/html\r\n"
			 << "\r\n";
		content = FCGX_GetParam("REQUEST_URI", request.envp);
		if(content=="/FCGI"){
			content = get_request_content(request); //optimize this function.
			json = true;
		}
		else{
			content = content.substr(6); //make sure nginx will pass string which it's length must be > 6. (location ^~ /FCGI/~)
			json = false;
		}
		// OK
		callback_index = content.back();
		if(callback_index=='\0'){
			cout << "0{\"Error\":\"No content\"}0";
			continue;
		}
		content.pop_back();
		if(json==true){
			index.extract_index_JSON(content,socket_index);
		}
		else{
			index.extract_index_GET(content,socket_index);
		}
		if(socket_index>0 && socket_index<max){//index must start from 1.
			//auth for node access
			if(client.fire(socket_index,content)>0){
				content = "0";
				content.push_back(callback_index);
				cout << content;
			}
			else{
				if(content==""){
					content = "0";
				}
				content.push_back(callback_index);
				cout << content;
			}
		}
		else{
			content = "0{\"Error\":\"Wrong socket index\"}";
			content.push_back(callback_index);
			cout << content;
		}
    }

    // restore stdio streambufs
    cin.rdbuf(cin_streambuf);
    cout.rdbuf(cout_streambuf);
    cerr.rdbuf(cerr_streambuf);
	configure conf(__FILE__);
	conf.node_crash_log();
    return 0;
}
