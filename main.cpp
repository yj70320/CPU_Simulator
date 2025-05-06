#include <iostream>
#include <fstream>
#include <vector>
#include <string>

// FLAG
#define Z 1 
#define C 2 
#define V 4 
#define N 8 

// 定义寄存器和内存 // Initialization
int AR = 0, PC = 0, DR = 0, IR = 0, TR = 0, AC = 0, R = 0, Flag = 0;
int high = 0;
int low = 0;
bool code_stop = 0;
int qty_line = 0, old = 128;
std::vector<int> M(256, 0);  // 256字节的内存

void update_flag(){
    // Z = 1
    if ((AC & 0xff) == 0) {
        Flag |= Z; 
    } else {
        Flag &= ~Z; 
    }    

    // C = 2
    if (AC < 0) {
        Flag |= C; 
    } else {
        Flag &= ~C; 
    }     

    // V = 4
    if (old <= 127 && AC >= 128){
        Flag |= V;
    }

    // N = 8
    if (AC > 127 && AC < 256) {
        Flag |= N;
    }
    else {        
        Flag &= ~N;
    }        
    old = 128;

}

// 取指阶段
void fetch() {
    std::cout << std::endl;
    AR = PC;
    std::cout << "fetch 1: AR = " << AR << "  PC = " << PC << std::endl;

    DR = M[AR];
    PC = PC + 1;
    std::cout << "fetch 2: DR = " << M[AR] << "  PC = " << PC << std::endl;

    IR = DR;
    AR = PC;
    std::cout << "fetch 3: IR = " << IR << "  AR = " << AR << std::endl;    
}

void nop() {
    std::cout << "nop instruction" << std::endl;
}

void ldac() {
    std::cout << "ldac instruction" << std::endl;
    DR = high;
    PC++;
    AR++;
    std::cout << "ldac1:     DR = " << DR <<"   PC = " << PC <<"   AR = "<< AR << std::endl;
    TR = DR;                     
    DR = low;
    PC++;
    std::cout << "ldac2:     TR = " << TR <<"   DR = " << DR <<"   AR = "<< AR << "   PC = "<< PC << std::endl;
    AR = (TR << 8) + DR;
    std::cout << "ldac3:     AR = " << AR <<std::endl;
    DR = M[AR];
    std::cout << "ldac4:     DR = " << DR <<std::endl;
    AC = DR;
    std::cout << "ldac5:     AC = " << AC <<std::endl;
}

void clac(){
    std::cout << "clac instruction" << std::endl; 
    AC = 0;
    update_flag();
    std::cout << "clac1:     AC = " << AC << " FLAG = " << Flag << std::endl;
}

void mvac(){
    std::cout << "mvac instruction" << std::endl;
    R = AC;
    std::cout << "mvac1" << " R = " << R << std::endl;
}

void movr(){
    std::cout << "movr instruction" << std::endl;
    AC = R;
    std::cout << "movr1" << " AC = " << AC << std::endl;
}

void add(){
    std::cout << "add instruction" << std::endl;
    AC = AC + R;
    update_flag();
    std::cout << "add1 AC = " << AC << " FLAG = " << Flag << std::endl;
}

void sub(){
    std::cout << "sub instruction" << std::endl;
    AC = AC - R;    
    update_flag();    
    std::cout << "sub1 AC = " << AC << " FLAG = " << Flag << std::endl;
}

void inac(){
    std::cout << "inac instruction" << std::endl;
    old = AC;
    AC++;
    update_flag();
    std::cout << "inac1 AC = " << AC << " FLAG = " << Flag << std::endl;
}

void f_and(){
    std::cout << "and instruction" << std::endl;
    AC = AC & R;
    update_flag();
    Flag &= ~C; 
    Flag &= ~V; // AND不会触发FLAG CV
    std::cout << "and1 AC = " << AC << " FLAG = " << Flag << std::endl;
}

void f_or(){
    std::cout << "or instruction" << std::endl;
    AC = AC | R;
    update_flag();
    Flag &= ~C; 
    Flag &= ~V; // OR不会触发FLAG CV
    std::cout << "or1 AC = " << AC << " FLAG = " << Flag << std::endl;
}

void f_xor(){
    std::cout << "xor instruction" << std::endl;
    AC = AC ^ R;
    update_flag();
    Flag &= ~C; 
    Flag &= ~V; // XOR不会触发FLAG CV
    std::cout << "xor1 AC = " << AC << " FLAG = " << Flag << std::endl;
}

void f_not(){
    std::cout << "not instruction" << std::endl;
    AC = (~(AC & 0xff)) & 0xff;
    update_flag();
    Flag &= ~C; 
    Flag &= ~V; // NOT不会触发FLAG CV
    std::cout << "not1 AC = " << AC << " FLAG = " << Flag << std::endl;
}

void mvi(){
    std::cout << "mvi instruction" << std::endl;    
    DR = AC;
    PC++;
    AR++;    
    std::cout << "mvi1 DR = " << DR << " PC = " << PC << " AR = " << AR << std::endl;
    AC = DR;
    std::cout << "mvi2 AC = " << AC << std::endl;
    // update_flag(); // MVI不会触发FLAG
}

void stac(){
    std::cout << "stac instruction" << std::endl;
    DR = high;
    PC++;
    AR++;
    std::cout << "stac1 DR = " << DR << " PC = " << PC << " AR = " << AR << std::endl;
    TR = DR;
    DR = low;
    PC++;
    std::cout << "stac2 TR = " << TR << " DR = " << DR << " PC = " << PC << std::endl;
    AR = (TR << 8) + DR;
    std::cout << "stac3 AR = " << AR << std::endl;
    DR = AC;
    std::cout << "stac4 DR = " << DR << std::endl;
    M[AR] = DR;
    std::cout << "stac5 M[AR] = " << M[AR] << std::endl;
}

// 解码阶段
void decode() { 
    switch(IR) {
        case 0x00:  // NOP
            nop();
            break;
        case 0x01:  // LDAC            
            ldac();
            break;
        case 0x02:  // STAC
            stac();
            break;
        case 0x03:  // MVAC            
            mvac();
            break;
        case 0x04:  // MOVR            
            movr();
            break; 
        case 0x08:  // ADD            
            add();
            break; 
        case 0x09:  // SUB            
            sub();
            break;
        case 0x0a:  // INAC
            inac();
            break;  
        case 0x0b:  // CLAC
            clac();
            break;     
        case 0x0c:  // AND
            f_and();
            break;
        case 0x0d:  // OR
            f_or();
            break;
        case 0x0e:  // XOR
            f_xor();
            break;
        case 0x0f:  // NOT
            f_not();
            break;
        case 0x16:  // MVI
            mvi();
            break;             
        case 0xff:  // NOP            
            code_stop = 1; // File ending
            break;
        default:
            std::cout << "Unknown instruction: " << IR << std::endl;
            break;
    }
}

int main() { 
    std::cout << "This code is created by Yijun Wu.\n";
    std::cout << "It's a CPU Simulator.\nThe CPU has 8-bit op codes and 16-bit addresses.\n\n";

    std::string filename;
    std::cout << "Enter the name of the file containing the program: ";
    std::getline(std::cin, filename);
    std::ifstream file(filename);

    // 检查文件是否成功打开
    if (file.is_open())
    {
        std::cout << "The " << filename << " opened properly." << std::endl;

        std::string line;
        int address = 0;

        while (std::getline(file, line)) //读取所有行
        {
           M[address] = std::stoi(line, nullptr, 16);  // 将十六进制字符串转换为整数
           address++;
           qty_line++;
        }       

        for (int i = 0; i < qty_line; i++)
        {
            if (M[i] == 0x01 || M[i] == 0x02)
            {                                          
                high = M[i+1];     
                low = M[i+2];
                i = i + 2; // 跨过两行
                fetch();
                decode();
            }
            else if (M[i] == 0x16) 
            {
                AC = M[i+1];
                i++;
                fetch();
                decode();
            }
            else
            {
                fetch();
                decode();                
            }  
            std::cout << "Instruction execution complete: AC = " << AC;
            std::cout << "  R = " << R << "  flag = " << Flag << "  AR = "<< AR;
            std::cout << "  PC = " << PC << "  DR = " << DR << std::endl;
            if (code_stop) 
            {
                code_stop = 0;
                break;
            }                
        }  
    } 
    else std::cerr << "Warning: Unable to open file " << filename << std::endl;     

    return 0;
}
