#include<iostream>
#include<math.h>
#include<vector>
#include<algorithm>
#include<set>
#include<map>
using namespace std;

bool isPowerofTwo(int n)
{
    if (n <= 0)
        return 0;
    if ((n & (~(n - 1))) == n)
        return 1;
    return 0;
}

int getValidIP(int* IP, char* chk, int& prf){
    cout << "Enter a Network IP form like 192.168.20.0/24\n";
    for(int i = 0 ; i < 4; i++){
        cin >> IP[i]; cin.clear(); cin >> chk[i];
    }
    cin >> prf;
    if(prf > 32 || prf <= 0 || chk[3] != '/'){
        return 0;
    }
    for(int i = 0 ;i < 4; i++){
        if(IP[i] > 255 || IP[i] < 0){
            return 0;
        }
        if(i < 3 && chk[i] != '.'){
            return 0;
        }
    }
    if(prf == 32){
        cout << "the network has only a single IPv4 address.\n";
        return 2;    
    }
    return 1;
}

void PrintIP(vector<vector<int>>& v){
    for(int j = 0; j < v.size(); j++){
        for(int k = 0; k < 4; k++){
            cout << v[j][k];
            if(k < 3) cout << '.';
        } 
        cout << '/' << v[j][4] << '\n';
    }
    cout << '\n';
}

vector<vector<int>> TraditionalAddr(int* IP, int& prf, int& subnets, int& subnetPortion){
    vector<vector<int>> ret(subnets, vector<int>(5));
    for(auto& i : ret){
        for(int j = 0; j < 4; j++){
            i[j] = IP[j];
        }
    }
    int it = 1;
    int stidx = (prf+subnetPortion)/8;
    int ed = prf % 8;
    int st = (prf+subnetPortion)%8;
    int comp = (subnetPortion - (st + ed))/8;
    int n = 2 + comp;
    ret[0][4] = prf + subnetPortion;
    for(int i = 1; i < (1<<subnetPortion); i++){
        for(int j = 0; j < n; j++){
            for(int k = 0; k < st; k++){
                if((1<<k)&i){
                    ret[it][stidx] |= (1<<(8-st+k));
                }
            }
            for(int k = 0; k < ed; k++){
                if(((1<<(k+st+8*comp))&i)){
                    ret[it][stidx-n+1] |= (1<<k);
                }
            }
            for(int k = 0; k < 8; k++){
                if(((1<<(k+st+(j-1)*8))&i)){
                    ret[it][stidx-j] |= (1<<k);
                }
            }
        }
        ret[it][4] = prf + subnetPortion;
        it++;
    } 
    return ret;
}

void VLSM(int* IP, int* h, int* frq, int& n, vector<vector<vector<int>>>& v, int prf){
    for(int i = 32; i >= 0; i--){
        int p = frq[i];
        if(!p) continue;
        int en = p;
        int mx = 32-prf;
        int step = max(mx - en, 1);
        int subnets = (1<<step);
        int subnetPortion = step;
        vector<vector<int>> ret = TraditionalAddr(IP, prf, subnets, subnetPortion);
        prf += subnetPortion;
        for(int j = 0; j < 4; j++){
            IP[j] = ret[1][j];
        }
        v[p] = ret; 
    }
}

int main(){
    int IP[4];
    char chk[4];
    int prf = 0;

    while(true){
        int ret = getValidIP(IP, chk, prf);
        if(ret == 1) break;
        else if(ret == 2) return 0;
        else cout << "Not valid IP!" << endl;
    }

    int op = 0;
    while(op != 1 && op != 2){
        cout << "1. Traditional subnetting\n2. VLSM\nChoose (1 or 2)\n";
        cin >> op;
    }
    if(op == 1){
        int mxsubnets = 1<<(32-prf);
        int subnets = 0;
        cout << "Enter number of Subnets\n"; 
        cin >> subnets;

        while(mxsubnets < subnets || !isPowerofTwo(subnets)){
            cout << "Number must be power of 2 and less or equal than " << mxsubnets << "take:" << '\n';
            cin >> subnets;
        }

        int subnetPortion = log2(subnets);
        vector<vector<int>> res = TraditionalAddr(IP, prf, subnets, subnetPortion);
        PrintIP(res);
    }
    else{
        int n;
        cout << "Enter number of networks\n";
        cin >> n;
        cout << "Enter number of Hosts in each network\n";

        int h[n];
        int frq[35] = {0};        
        for(int i = 0 ;i < n; i++){
            cin >> h[i];
            int temp = log2(h[i])+1;
            frq[temp] = temp;
        }
        sort(h, h+n);
        reverse(h, h+n);

        vector<vector<vector<int>>> v(35);
        VLSM(IP, h, frq, n, v, prf);

        for(int i = 32 ;i >= 0; i--){
            int p = frq[i];
            if(!p) continue;
            cout << "For hosts from " << (1<<(p-1))-2 << " to " << (1<<p)-2 << " take:\n";
            PrintIP(v[p]);
        }
    }
}
