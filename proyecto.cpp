#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <fcntl.h>
#include "Learner.h"
/* usando otra funcion clear */
#define clear() printf("\033[H\033[J")

Learner* learner;

/* To display header shell environment */
void header() {
    char cwd[1024];
    char *user;
    user = getenv("USER");
    getcwd(cwd, sizeof(cwd));
    cout << user << "@" << "computer" << ":" << cwd << "$ ";
}

/* Counts spacing for pipes */
int contarEspacios(char *cadena){
    int retorno = 0;
    for (int i = 0; i < strlen(cadena); i++){
        if (cadena[i] == ' '){
            retorno += 1;
        }
    }
    return retorno;
}

/* Function to execute Cat command to write to file */
void executeStdoutCatCommand(string filename) {
    int outfd = open(filename.c_str(), O_CREAT|O_WRONLY|O_TRUNC, 0644);
    if (!outfd) {
        cout << "FILE NOT IN DIRECTORY" << endl;
        perror("open");
        cout << endl;
        return;
    }
    pid_t pid = fork();
    if (pid < 0) {
        close(outfd);
        cout << "COULD NOT FORK" << endl;
        perror("fork");
        cout << endl;
        return;
    }
    if (pid == 0) {
        cout << "//Ctrl+d to reach EOF" << endl;
        dup2(outfd, 1); 
        execlp("cat", "cat", NULL);
        close(outfd);
    }
    wait(NULL);
    return;
}

/* Function to execute cat comand in terminal */
void executeSimpleCatCommand(char** args) {
    pid_t pidt = fork();
    if (pidt < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pidt == 0) {
        int success = execlp(args[0], args[0], args[1], NULL);
        if (success < 0) {
            cout << endl;
            perror("");
            cout << endl;
        }
    }
    wait(NULL);
    return;
}

/* Function to execute simple command (no piping) */
void executeSimpleCommand(char** args){
    pid_t pidt = fork();
    if(pidt == 0) {
        int exito = execvp(args[0],args);
        if (exito < 0){
            perror("Command stops");
            cout << "Command \'" << args[0] << "\' was not found." <<endl;
            string wdym;
            string suggestion = learner -> Suggest(args[0]);
            if (!suggestion.empty()) {
                cout << "Did you mean: " << suggestion << "? [Y/n]" << endl;
                string yay_nay;
                cin >> yay_nay;
                if(yay_nay == "n" || yay_nay == "N") {
                    cout << "What did you mean?";
                    cin >> wdym;
                    //learner -> specifyConfidence(wdym, args[0], -1);
                    cout << "Thank you for your feedback." << endl;
                } else {
                    //learner -> specifyConfidence(suggestion, args[0], 1);
                }
                wait(NULL);
                return;
            } else {
                cout << "What did you mean?";
                cin >> wdym;
                if(!learner -> commandExists(wdym)) {
                    learner -> pushValue(wdym, args[0]);
                }
            }
        }
    }
    wait(NULL);
    return;
}

/* Function to count needed pipes */
int contarPipes(char** args, int size){
    int cont = 0;
    for(size_t i = 0; i < size; i++){
        if(!strcmp(args[i], "|")){
            cont++;            
        }
    }
    
    return cont;
}

char*** dosComandos(char** args, int size){
    char* arg1[100];
    char* arg2[100];

    for(int i; i < 100; i++){
        arg1[i] = NULL;
        arg2[i] = NULL;
    }

    char** argsO[2];
    argsO[0] = arg1;
    argsO[1] = arg2;

    char*** argsOut = argsO;

    bool pie = false;
    int cont = 0;

    for(size_t i = 0; i < size; i++)
    {
        if(pie){
            argsOut[1][cont] = args[i];
            cont++;
        }else{
            if(!strcmp(args[i], "|")){
                pie = true;
                argsOut[0][i] = NULL;
            }else{
                argsOut[0][i] = args[i];
            }
        }
    }
    argsOut[1][cont] = NULL;    
    return argsOut;
}

void processSimplePipe(char** args, char** args2) {
    int pipefd[2];
    pipe(pipefd);

    if (fork() == 0){
        close(pipefd[0]);

        dup2(pipefd[1], 1);
        dup2(pipefd[1], 2);

        close(pipefd[1]);

        execvp(args[0], args);
        perror("execvp de args falló");
        exit(1);
    }else{
        const int buffer_size = 1024;
        char buffer[buffer_size];

        close(pipefd[1]);

        while (read(pipefd[0], buffer, sizeof(buffer)) != 0){
        }

        if(fork() == 0){
            //execvp(args2[0], args2, const_cast<char *>(buffer));
            execlp(args2[0], args2[0], buffer, 0);
            //execlp("cowsay", "cowsay", "ola", 0);
            perror("execlp de args2 falló");
        }
        
        wait(0);
        wait(0);
    }
}

void processComplexPipe(char **args, char **args2)
{
    int conts = 0;

    try{
        while(args2[conts] != NULL){
            conts++;
            //cout << "cont -> " << conts << endl;                                                                        
        }
    }catch(...){
        perror("Error contango argumentos agrs2");
    }

    int pipefd[2];
    pipe(pipefd);

    if (fork() == 0){
        close(pipefd[0]);

        dup2(pipefd[1], 1);
        dup2(pipefd[1], 2);

        close(pipefd[1]);

        execvp(args[0], args);
        perror("execvp de args falló");
        exit(1);
    }else{
        const int buffer_size = 1024;
        char buffer[buffer_size];

        close(pipefd[1]);

        while (read(pipefd[0], buffer, sizeof(buffer)) != 0){
        }

        args2[conts] = buffer;
        args2[conts + 1] = NULL;

        if(fork() == 0){
            execvp(args2[0], args2);
            perror("execlp de args2 falló");
        }
        
        wait(0);
        wait(0);
    }
}

void executeSimplePipe(char*** args){
    int des_p[2];
    char** args1 = args[0];
    char** args2 = args[1];

    processComplexPipe(args1, args2);
}

char* processToBuffer(char **args)
{
    int pipefd[2];
    pipe(pipefd);

    const int buffer_size = 1024;
    char buffer[buffer_size];

    //cout << "Procesando a buffer: " << args[0] << endl;

    if (fork() == 0)
    {
        close(pipefd[0]);

        dup2(pipefd[1], 1);
        dup2(pipefd[1], 2);

        close(pipefd[1]);

        if (args[1] == NULL || args[0] == "ls") {
            execlp(args[0], args[0], NULL);
        }else{
            execlp(args[0], args[0], args[1], NULL);
        }
        
        
        
        perror("execlp a buffer falló");
        exit(1);
    }
    else
    {
        wait(0);
        close(pipefd[1]);

        while (read(pipefd[0], buffer, sizeof(buffer)) != 0) {
            //emptying buffer...
        }
    }

    int bufSize = strlen(buffer);
    char *sanitizedBuffer = new char[bufSize];

        for (size_t i = 0; i < bufSize; i++)
    {
        sanitizedBuffer[i] = buffer[i];
    }

    return sanitizedBuffer;
}

void executeMultiplePipe(char*** args, int size){
    int des_p[2];
    char *buffer;

    for(int i = 0; i < size; i++)
    {
        char *argsIn[3];
        argsIn[0] = args[i][0];

        if (i == 0){
            argsIn[1] = NULL;
            argsIn[2] = NULL;
        }else{
            argsIn[1] = buffer;
            argsIn[2] = NULL;
        }

        //cout << "Out right now: " << out;

        if (i == size - 1){
            if (fork() == 0)
            {
                //execvp(args2[0], args2, const_cast<char *>(buffer));

                execvp(argsIn[0], argsIn);
                //execlp("cowsay", "cowsay", "ola", 0);
                perror("Error ejecutando pipe multiple");
            }
            else
            {
                wait(0);
            }
            
        }else{
            buffer = processToBuffer(argsIn);
        }

        
    }
}

/* Master function for CMD control */
void interpretCmd(){
    char *buffer;
    buffer = readline("");
    string historia = buffer;

    bool seguir = true;
    char* args[100];
    for(int i = 0; i < 100 ; i++){
        args[i] = 0;
    }
    int posActual = 0;
    int espacios = contarEspacios(buffer);

    if (espacios == 0) {
        args[0] = buffer;
    } else{
        while (posActual <= espacios) {
            args[posActual] = strsep(&buffer, " ");
            posActual++;
        }
        args[posActual] = NULL;
    }
    if (strlen(historia.c_str()) > 0) {
        add_history(historia.c_str());
        //logged command
        if (strcmp(args[0], "exit") == 0 || strcmp(args[0], "close") == 0) {
            int ar_place = 0;
            while (ar_place < historia.size()) {
                cout << "quitting..." << endl;
                exit(0);
                ar_place++;
            }    
        }
        else if (strcmp(args[0], "clear") == 0) {
            clear();
        }
        else if (strcmp(args[0], "cd") == 0) {
            if (chdir(args[1]) < 0) {
                cout << "cd to " << args[1] << " no se pudo ejecutar.";
                perror(":>");
                cout << "\n";
            }
        } else if (strcmp(args[0], "cat")==0) {
            if (args[1] == NULL) {
                cout << "You didn't specify a file!" << endl;
            } else if (strcmp(args[1],">")==0) {
                if (args[2] == NULL) {
                    cout << "You didn't specify a file!" << endl;
                } else {
                    string filename = args[2]; 
                    executeStdoutCatCommand(filename);
                }
            } else {
                executeSimpleCatCommand(args);
                cout << endl;
            }
        } else if (contarPipes(args, posActual) == 1){
            char*** comandosPipe;
            comandosPipe = dosComandos(args, posActual);

            executeSimplePipe(comandosPipe);

        }else if (contarPipes(args, posActual) > 1){
            int pipeCount = contarPipes(args, posActual);

            int tokCont = 0;
            int cmdCont = 0;
            int paramCont = 0;

            char* tok = args[tokCont];
            char*** cmdArray = new char**[1024];

            for(size_t i = 0; i < 1024; i++)
            {
                cmdArray[i] = new char*[pipeCount + 1];
            }

            for(int i = 0; i < pipeCount*2 + 1; i++)
            {
                char *tok = args[i];
                //cout << "Tok " << i << ": " << tok << endl;

                if (strstr(tok, "|") != NULL) {
                    cmdCont ++;
                    paramCont = 0;
                }else{
                    cmdArray[cmdCont][paramCont] = tok;
                    paramCont ++;
                }
            }
            
            executeMultiplePipe(cmdArray, pipeCount + 1);

        }else{
            

            executeSimpleCommand(args);
        }
    }
}
// function for finding pipe 
int parsePipe(char* str, char** strpiped) { 
    int i; 
    for (i = 0; i < 2; i++) { 
        strpiped[i] = strsep(&str, "<"); 
        if (strpiped[i] == NULL) {
            break; 
        }
    } 

    if (strpiped[1] == NULL) {
        return 0; // returns zero if no pipe is found. 
    } else {
        return 1; 
    } 
}

int main(){
    learner = new Learner();
    while (true){
        header();
        interpretCmd();
    }
    return 0;
}