#include <string>
#include <vector>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

using namespace std;

vector<string> split(const string &str, const string &pattern) {
    vector<string> res;
    if (str.empty())
        return res;
    size_t i = 0;
    while (true) {
        size_t p = str.find(pattern, i);
        if (p == std::string::npos) {
            res.push_back(str.substr(i));
            break;
        }
        res.push_back(str.substr(i, p - i));
        i = p + 1;
    }
    return res;
}

char buf[1000] = {0};

int shell(const string &sh) {
    FILE *fp;
    char buf_ps[1024];
    memset(buf, 0, sizeof(buf));
    if ((fp = popen((sh + " 2>&1").c_str(), "r")) != nullptr) {
        while (fgets(buf_ps, sizeof(buf_ps), fp) != nullptr) {
            strcat(buf, buf_ps);
            if (strlen(buf) > 1024) {
                break;
            }
        }
        pclose(fp);
        return 0;
    }
    return -1;
}

void start(const string &name) {
    string sudo = "echo '-' | sudo -S ";
    shell(sudo + "echo 'success'");
    if (strcmp(buf, "success\n") != 0) {
        while (true) {
            char pwd[1000] = {0};
            printf("密码: ");
            scanf("%s", pwd);
            string sp = pwd;
            sudo = "echo '" + sp + "' | sudo -S ";
            shell(sudo + "echo 'success'");
            if (strcmp(buf, "Password:success\n") == 0) {
                break;
            }
            printf("密码输入有误, 请重新输入\n");
        }
    }
    printf("虚拟机启动中...\n");
    shell("open -a /Applications/Parallels\\ Desktop.app/Contents/MacOS/prl_client_app");
    sleep(3);
    shell(sudo + "date 090100002021");
    shell("prlctl status '" + name + "'");
    if (!strstr(buf, "running")) {
        shell("prlctl start '" + name + "'");
        sleep(1);
        if (!strstr(buf, "running")) {
            shell("prlctl start '" + name + "'");
        }
    }
    shell(sudo + "sntp -sS time.apple.com");
    printf("虚拟机启动完成");
}

int main() {
    shell("prlctl list -ao name");
    vector<string> vmArr = split(buf, "\n");
    if (vmArr.size() == 2) {
        printf("无可用虚拟机\n");
        return 0;
    }
    printf("//== 可用虚拟机%lu个 =====================\n", vmArr.size() - 2);
    printf("||\n");
    for (vector<int>::size_type i = 1; i < vmArr.size() - 1; i++) {
        printf("||  %lu. %s\n", i, vmArr[i].c_str());
    }
    printf("||\n");
    printf("\\\\====================================\n");
    while (true) {
        printf("输入虚拟机序号启动: ");
        char in[1000];
        scanf("%s", in);
        char *leftover;
        long i = strtol(in, &leftover, 0);
        if (i >= 1 && i < vmArr.size() - 1) {
            start(vmArr[i]);
            return 0;
        }
        printf("不存在序号为'%s'的虚拟机, 请重新输入\n", in);
    }
}

