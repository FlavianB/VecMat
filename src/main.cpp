#include <iostream>
#include <fstream>
#include <graphics.h>
#include <mmsystem.h>
#include <winbgim.h>
#include <cmath>

using namespace std;

bool Running = true; //flag pentru rularea programului
bool startVector = false; //ecranul de vectori
bool startMatrix = false; //ecranul de matrici
bool startInstructions = false; //ecranul de instructiuni
bool startSettings = false; //ecranul de setari
bool Exit = false; //in cazul in care progarmul este inchis
int countV, countLin, countCol, countCol2;//nr de elemente din vector, din matrice
bool requestChV = false, requestChM = false;
char functionCall_v[100]; //sir de caractere ajutator folosit in functiile de mai jos
bool SumButtons = false; //flag pentru afisarea butoanelor de sume
bool showMatrix2 = false, showMatrixResultSum = false, showMatrixResultMult = false, FilledMatrix = false, showMatrixResultTrans = false;
void createVectorBox(char element[], int);
void mouseTrackerStart(), mouseTrackerVector(), mouseTrackerMatrix(), mouseTrackerSettings();
void StartScreen(), VectorScreen(), MatrixScreen(), SettingsScreen(), InstructionsScreen();
void animateVbox(int, int), exitanimateVbox(int);
void Check_ShowResultMult(), Check_ShowResultSum();

struct Button
{
    int pozX;
    int pozY;

    int height;
    int width;

    bool mouseOver = false;
    bool show = false;
    bool press = false;

    int value = 0;

    struct Text
    {
        int pozX;
        int pozY;
        char buttonText[100];
    } text;
};

Button ButtonVector, ButtonMatrix, ButtonInstructions, ButtonSettings, ButtonExit;
Button ButtonSum_V, ButtonMult_V, ButtonScalar_V, ButtonSavedFile_V, ButtonBubbleSort_V, ButtonQuickSort_V, ButtonShift_R, ButtonShift_L, ButtonElim_V;
Button ButtonNLin_M, ButtonNCol_M, ButtonSum_M, ButtonMult_M, ButtonDif_M, ButtonScalar_M, ButtonDet_M, ButtonInv_M, ButtonTrans_M, ButtonSumElem_M, ButtonSumDiagP, ButtonSumDiagP_U, ButtonSumDiagP_A, ButtonSumDiagS, ButtonSumDiagS_U, ButtonSumDiagS_A, ButtonSavedFile_M, ButtonPower_M;
Button ButtonDarkTheme, ButtonMute, ButtonSlowAnim;

struct Settings
{
    struct bkTheme
    {
        int dark = RGB(37, 23, 73);
        int light = RGB(255, 251, 235);
    } bkColor;

    struct textTheme
    {
        int dark = RGB(255, 251, 235);
        int light = RGB(37, 23, 73);
    } textColor;

    struct highlightTheme
    {
        int dark = RGB(0, 130, 255);
        int light = RGB(254, 208, 73);
    } highlightColor;

    struct animSpeed
    {
        int slow = 700;
        int fast = 220;
    } animationSpeed;

    int fastAnim = true; //settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow
    int textFont = EUROPEAN_FONT;
    bool playFXSounds = true;
    bool darkTheme = false; //settings.darkTheme?settings.bkColor.dark:settings.bkColor.light
} settings;

const int screenHeight = GetSystemMetrics(SM_CYSCREEN);
const int screenWidth = GetSystemMetrics(SM_CXSCREEN);

struct Mouse
{
    int x;
    int y;
} mouse;

struct VBox
{
    int pozX = screenWidth/3;
    int pozY = 65;
    int width;
    int height = 70;
    int value = 0;
    char text[100] = "_";
    bool show = false;
};
VBox VectorBox[1000];

struct MBox
{
    int pozX = 450;
    int pozY;
    int width = 70;
    int height = 50;
    int value = 0;
    char text[100] = "_";
    bool show = false;
    int color = settings.darkTheme?settings.textColor.dark:settings.textColor.light;
};
MBox MatrixBox[20][20], MatrixBox2[20][20], MatrixBoxResult[20][20];

int ChartoNumber(char a[]) //functie pentru a transforma un sir de caractere intr-un numar
{
    int n = 0;
    if (a[0] != '-')
        for (int i = 0; i<strlen(a); a++)
        {
            n = n*10 + (int)a[i] - 48;
        }
    else
    {
        for (int i = 1; i<strlen(a); a++)
        {
            n = n*10 + (int)a[i] - 48;
        }
        n = -n;
    }
    return n;
}

void NumbertoChar(int n, char *functionCall_v) //functie pentru a transforma un numar intr-un sir de caractere
{
    int auxs = 0, nrc = 0;

    if (n>0)
        auxs = n;
    else
        auxs = -n;
    while (auxs)
        auxs = auxs/10, nrc++;

    auxs = -n;
    if (n==0)
        functionCall_v[0] = '0', functionCall_v[1] = '\0';
    else
    {
        if (n>0)
        {
            for (int i = nrc-1; i>=0; i--)
                functionCall_v[i] = n%10 + 48, n = n/10;
            functionCall_v[nrc] = '\0';
        }
        else
        {
            for (int i = nrc; i>=1; i--)
                functionCall_v[i] = auxs%10 + 48, auxs = auxs/10;
            functionCall_v[0] = '-';
            functionCall_v[nrc+1] = '\0';
        }
    }
    return;
}

bool verifyButton(Button b) //verifica apasarea unui buton
{
    if (mouse.x >= b.pozX && mouse.y >= b.pozY && mouse.x <= b.pozX + b.width && mouse.y <= b.pozY + b.height)
        return true;
    return false;
}

bool hoverButton(Button b) //verifica daca mouse-ul se afla peste un anumit buton
{
    if (mousex() >= b.pozX && mousey() >= b.pozY && mousex() <= b.pozX + b.width && mousey() <= b.pozY + b.height)
        return true;
    return false;
}

void hoverVbox() //verifica daca mouse-ul se afla peste o casuta a vectorului
{
    for (int i = 0; i<=10; i++)
        if (mousex() >= VectorBox[i].pozX && mousey() >= VectorBox[i].pozY && mousex() <= VectorBox[i].pozX + VectorBox[i].width && mousey() <= VectorBox[i].pozY + VectorBox[i].height)
            animateVbox(i, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
        else exitanimateVbox(i);
}

void drawButton(Button b) //desenarea unui buton
{
    rectangle(b.pozX, b.pozY, b.width + b.pozX, b.height + b.pozY);
}

void animateVbox(int pos, int culoare) //animatia unei casute din vector
{
    setcolor(culoare);
    setlinestyle(0, 0, 3);
    rectangle(VectorBox[pos].pozX, VectorBox[pos].pozY, VectorBox[pos].width + VectorBox[pos].pozX, VectorBox[pos].height + VectorBox[pos].pozY);
    delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
    setlinestyle(0, 0, 3);
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
}

void exitanimateVbox(int pos)
{
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
    setlinestyle(0, 0, 3);
    rectangle(VectorBox[pos].pozX, VectorBox[pos].pozY, VectorBox[pos].width + VectorBox[pos].pozX, VectorBox[pos].height + VectorBox[pos].pozY);
}

void animateMbox(int i, int j, int culoare) //animatia unei casute din matrice
{
    setcolor(culoare);
    setlinestyle(0, 0, 3);
    rectangle(MatrixBox[i][j].pozX, MatrixBox[i][j].pozY, MatrixBox[i][j].width + MatrixBox[i][j].pozX, MatrixBox[i][j].height + MatrixBox[i][j].pozY);
    delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
    setlinestyle(0, 0, 3);
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
}

void exitanimateMbox(int i, int j)
{
    setcolor(MatrixBox[i][j].color);
    setlinestyle(0, 0, 3);
    rectangle(MatrixBox[i][j].pozX, MatrixBox[i][j].pozY, MatrixBox[i][j].width + MatrixBox[i][j].pozX, MatrixBox[i][j].height + MatrixBox[i][j].pozY);
}

void animateMbox2(int i, int j, int culoare)
{
    setcolor(culoare);
    setlinestyle(0, 0, 3);
    rectangle(MatrixBox2[i][j].pozX, MatrixBox2[i][j].pozY, MatrixBox2[i][j].width + MatrixBox2[i][j].pozX, MatrixBox2[i][j].height + MatrixBox2[i][j].pozY);
    delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
    setlinestyle(0, 0, 3);
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
}

void exitanimateMbox2(int i, int j)
{
    setcolor(MatrixBox2[i][j].color);
    setlinestyle(0, 0, 3);
    rectangle(MatrixBox2[i][j].pozX, MatrixBox2[i][j].pozY, MatrixBox2[i][j].width + MatrixBox2[i][j].pozX, MatrixBox2[i][j].height + MatrixBox2[i][j].pozY);
}

void animateButton(Button b) //animatia unui buton
{
    setlinestyle(0, 0, 3);
    setcolor(settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
    rectangle(b.pozX, b.pozY, b.width + b.pozX, b.height + b.pozY);
    delay(400);
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
}

void exitanimateButton(Button b)
{
    setlinestyle(0, 0, 3);
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
    rectangle(b.pozX, b.pozY, b.width + b.pozX, b.height + b.pozY);
}


void Sum_V(char *functionCall_v, int n) //suma elemetelor din vector
{
    int s = 0, auxs = 0, nrc = 0;
    for (int i = 1; i<=n; i++)
    {
        s += VectorBox[i].value;
    }

    if (s>0)
        auxs = s;
    else
        auxs = -s;
    while (auxs)
        auxs = auxs/10, nrc++;

    auxs = -s;
    if (s==0)
        functionCall_v[0] = '0', functionCall_v[1] = '\0';
    else
    {
        if (s>0)
        {
            for (int i = nrc-1; i>=0; i--)
                functionCall_v[i] = s%10 + 48, s = s/10;
            functionCall_v[nrc] = '\0';
        }
        else
        {
            for (int i = nrc; i>=1; i--)
                functionCall_v[i] = auxs%10 + 48, auxs = auxs/10;
            functionCall_v[0] = '-';
            functionCall_v[nrc+1] = '\0';
        }
    }
}

void Mult_V(char *functionCall_v, int n) //produsul elementelor din vector
{
    int s = 1, auxs = 0, nrc = 0;
    for (int i = 1; i<=n; i++)
    {
        s *= VectorBox[i].value;
    }

    if (s>0)
        auxs = s;
    else
        auxs = -s;
    while (auxs)
        auxs = auxs/10, nrc++;

    auxs = -s;
    if (s==0)
        functionCall_v[0] = '0', functionCall_v[1] = '\0';
    else
    {
        if (s>0)
        {
            for (int i = nrc-1; i>=0; i--)
                functionCall_v[i] = s%10 + 48, s = s/10;
            functionCall_v[nrc] = '\0';
        }
        else
        {
            for (int i = nrc; i>=1; i--)
                functionCall_v[i] = auxs%10 + 48, auxs = auxs/10;
            functionCall_v[0] = '-';
            functionCall_v[nrc+1] = '\0';
        }
    }
}

void BubbleSort_V(VBox VectorBox[], int n) //sortarea elementelor din vector
{
    int i, j;
    for (i = 1; i <= n; i++)
        for (j = 1; j < n - i + 1; j++)
            if (VectorBox[j].value > VectorBox[j+1].value)
            {
                animateVbox(j, MAGENTA);
                animateVbox(j+1, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
                exitanimateVbox(j);
                delay(100);
                exitanimateVbox(j+1);
                delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
                swap(VectorBox[j].value, VectorBox[j+1].value), swap(VectorBox[j].text, VectorBox[j+1].text), swap(VectorBox[j].width, VectorBox[j+1].width);
                if(settings.playFXSounds)
                    PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);
                bar(VectorBox[1].pozX - 1, VectorBox[1].pozY - 1, screenWidth, VectorBox[1].pozY + VectorBox[1].height + 1);
                createVectorBox(VectorBox[1].text, 1);
                animateVbox(j, MAGENTA);
                animateVbox(j+1, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
                exitanimateVbox(j);
                delay(100);
                exitanimateVbox(j+1);
                delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
                bar(VectorBox[1].pozX - 1, VectorBox[1].pozY - 1, screenWidth, VectorBox[1].pozY + VectorBox[1].height + 1);
                createVectorBox(VectorBox[1].text, 1);
            }
}

void QuickSort_V(VBox VectorBox[], int st, int dr)
{
    bar(VectorBox[1].pozX - 20, VectorBox[1].pozY + VectorBox[1].height + 5, screenWidth, ButtonSum_V.pozY - 3);

    if(st < dr)
    {
        //pivotul este inițial v[st]
        int m = (st + dr) / 2;

        settextjustify(CENTER_TEXT, BOTTOM_TEXT);
        animateVbox(m, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
        outtextxy(VectorBox[st].pozX + VectorBox[st].width/2, VectorBox[st].pozY + VectorBox[st].height + 25, "^");
        outtextxy(VectorBox[st].pozX + VectorBox[st].width/2, VectorBox[st].pozY + VectorBox[st].height + 35, "|");
        outtextxy(VectorBox[st].pozX + VectorBox[st].width/2, VectorBox[st].pozY + VectorBox[st].height + 45, "st");

        animateVbox(m, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
        outtextxy(VectorBox[dr].pozX + VectorBox[dr].width/2, VectorBox[dr].pozY + VectorBox[dr].height + 25, "^");
        outtextxy(VectorBox[dr].pozX + VectorBox[dr].width/2, VectorBox[dr].pozY + VectorBox[dr].height + 35, "|");
        outtextxy(VectorBox[dr].pozX + VectorBox[dr].width/2, VectorBox[dr].pozY + VectorBox[dr].height + 45, "dr");
        delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);

        swap(VectorBox[st].value, VectorBox[m].value), swap(VectorBox[st].text, VectorBox[m].text), swap(VectorBox[st].width, VectorBox[m].width);
        bar(VectorBox[1].pozX - 20, VectorBox[1].pozY + VectorBox[1].height + 5, screenWidth, ButtonSum_V.pozY - 3);
        bar(VectorBox[1].pozX - 1, VectorBox[1].pozY - 1, screenWidth, VectorBox[1].pozY + VectorBox[1].height + 1);
        createVectorBox(VectorBox[1].text, 1);

        animateVbox(m, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
        if (m==st || m==dr || (m!=st && m!=dr))
        {
            outtextxy(VectorBox[m].pozX + VectorBox[m].width/2, VectorBox[m].pozY + VectorBox[m].height + 25, "^");
            outtextxy(VectorBox[m].pozX + VectorBox[m].width/2, VectorBox[m].pozY + VectorBox[m].height + 35, "|");
            outtextxy(VectorBox[m].pozX + VectorBox[m].width/2, VectorBox[m].pozY + VectorBox[m].height + 45, "piv");
        }

        //animateVbox(m, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
        if (m!=st)
        {
            outtextxy(VectorBox[st].pozX + VectorBox[st].width/2, VectorBox[st].pozY + VectorBox[st].height + 25, "^");
            outtextxy(VectorBox[st].pozX + VectorBox[st].width/2, VectorBox[st].pozY + VectorBox[st].height + 35, "|");
            outtextxy(VectorBox[st].pozX + VectorBox[st].width/2, VectorBox[st].pozY + VectorBox[st].height + 45, "st");
        }

        //animateVbox(m, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
        if(m!=dr)
        {
            outtextxy(VectorBox[dr].pozX + VectorBox[dr].width/2, VectorBox[dr].pozY + VectorBox[dr].height + 25, "^");
            outtextxy(VectorBox[dr].pozX + VectorBox[dr].width/2, VectorBox[dr].pozY + VectorBox[dr].height + 35, "|");
            outtextxy(VectorBox[dr].pozX + VectorBox[dr].width/2, VectorBox[dr].pozY + VectorBox[dr].height + 45, "dr");
        }
        delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);


        int i = st, j = dr, d = 0;
        while(i < j)
        {
            if(VectorBox[i].value > VectorBox[j].value)
            {
                animateVbox(i, LIGHTGREEN), animateVbox(j, YELLOW);
                exitanimateVbox(i), delay(100), exitanimateVbox(j);
                delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
                if(settings.playFXSounds)
                    PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);
                exitanimateVbox(m);
                swap(VectorBox[i].value, VectorBox[j].value), swap(VectorBox[i].text, VectorBox[j].text), swap(VectorBox[i].width, VectorBox[j].width);

                bar(VectorBox[1].pozX - 20, VectorBox[1].pozY + VectorBox[1].height + 5, screenWidth, ButtonSum_V.pozY - 3);
                bar(VectorBox[1].pozX - 1, VectorBox[1].pozY - 1, screenWidth, VectorBox[1].pozY + VectorBox[1].height + 1);
                createVectorBox(VectorBox[1].text, 1);

                animateVbox(m, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                if (m==st || m==dr || (m!=st && m!=dr))
                {
                    outtextxy(VectorBox[m].pozX + VectorBox[m].width/2, VectorBox[m].pozY + VectorBox[m].height + 25, "^");
                    outtextxy(VectorBox[m].pozX + VectorBox[m].width/2, VectorBox[m].pozY + VectorBox[m].height + 35, "|");
                    outtextxy(VectorBox[m].pozX + VectorBox[m].width/2, VectorBox[m].pozY + VectorBox[m].height + 45, "piv");
                }

                //animateVbox(m, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                if (m!=st)
                {
                    outtextxy(VectorBox[st].pozX + VectorBox[st].width/2, VectorBox[st].pozY + VectorBox[st].height + 25, "^");
                    outtextxy(VectorBox[st].pozX + VectorBox[st].width/2, VectorBox[st].pozY + VectorBox[st].height + 35, "|");
                    outtextxy(VectorBox[st].pozX + VectorBox[st].width/2, VectorBox[st].pozY + VectorBox[st].height + 45, "st");
                }

                //animateVbox(m, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                if(m!=dr)
                {
                    outtextxy(VectorBox[dr].pozX + VectorBox[dr].width/2, VectorBox[dr].pozY + VectorBox[dr].height + 25, "^");
                    outtextxy(VectorBox[dr].pozX + VectorBox[dr].width/2, VectorBox[dr].pozY + VectorBox[dr].height + 35, "|");
                    outtextxy(VectorBox[dr].pozX + VectorBox[dr].width/2, VectorBox[dr].pozY + VectorBox[dr].height + 45, "dr");
                }

                animateVbox(i, LIGHTGREEN), animateVbox(j, YELLOW);
                exitanimateVbox(i), delay(100), exitanimateVbox(j);
                delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
                exitanimateVbox(m);

                d = 1 - d;
            }
            i += d;
            j -= 1 - d;
        }
        QuickSort_V(VectorBox, st, i - 1);
        QuickSort_V(VectorBox, i + 1, dr);
    }
}

void Shift_R(VBox VectorBox[], int n)
{
    VBox aux[1];
    aux[0] = VectorBox[n];
    animateVbox(n, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
    delay(200);
    exitanimateVbox(n);
    for (int i = n; i>=2; i--)
        VectorBox[i] = VectorBox [i - 1];
    VectorBox[1] = aux[0];
    VectorBox[1].pozX = screenWidth/3;
    bar(VectorBox[1].pozX - 1, VectorBox[1].pozY - 1, screenWidth, VectorBox[1].pozY + VectorBox[1].height + 1);
    createVectorBox(VectorBox[1].text, 1);
    if(settings.playFXSounds)
        PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);
}

void Shift_L(VBox VectorBox[], int n)
{
    VBox aux[1];
    aux[0] = VectorBox[1];
    animateVbox(1, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
    delay(200);
    exitanimateVbox(1);
    if (n!=1)
    {
        for (int i = 1; i<n; i++)
            VectorBox[i].value = VectorBox [i + 1].value, VectorBox[i].width = VectorBox [i + 1].width, strcpy(VectorBox[i].text, VectorBox[i+1].text);
        VectorBox[n] = aux[0];
        VectorBox[n].pozX = VectorBox[n-1].pozX + VectorBox[n-1].width + 10;
        bar(VectorBox[1].pozX - 1, VectorBox[1].pozY - 1, screenWidth, VectorBox[1].pozY + VectorBox[1].height + 1);
        createVectorBox(VectorBox[1].text, 1);
    }
    if(settings.playFXSounds)
        PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);
}

void Elim_V(VBox VectorBox[], int pos, int &countV)
{
    for (int i = pos; i<countV; i++)
        VectorBox[i].value = VectorBox [i + 1].value, VectorBox[i].width = VectorBox [i + 1].width, strcpy(VectorBox[i].text, VectorBox[i+1].text);
    VectorBox[countV].value = 0;
    strcpy(VectorBox[countV].text, "_");
    VectorBox[countV].width = textwidth("_") + 20;
    countV--;
    bar(VectorBox[1].pozX - 1, VectorBox[1].pozY - 1, screenWidth, VectorBox[1].pozY + VectorBox[1].height + 1);
    createVectorBox(VectorBox[1].text, 1);
    //VectorBox[countV + 1].show = false;
}

void SumDiagP() //suma de pe diagonala principala
{
    bar(MatrixBox[4][4].pozX + MatrixBox[4][4].width + 10, MatrixBox[4][4].pozY - 4, screenWidth, MatrixBox[4][4].pozY + MatrixBox[4][4].height);
    functionCall_v[0] = '\0';
    int i, offset = MatrixBox[4][4].pozX + MatrixBox[4][4].width + 20, s;

    for (int k = 1; k<=countLin; k++)
    {
        s = 0;

        animateMbox(k, k, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
        setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
        settextstyle(settings.textFont, HORIZ_DIR, 3);
        settextjustify(LEFT_TEXT, TOP_TEXT);
        outtextxy(offset, MatrixBox[4][4].pozY, MatrixBox[k][k].text);
        delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
        offset = offset + textwidth(MatrixBox[k][k].text) + 10;
        if (k != countLin)
            outtextxy(offset, MatrixBox[4][4].pozY, "+"), offset = offset + textwidth("+") + 10;
        else
        {
            if(settings.playFXSounds)
                PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);
            outtextxy(offset, MatrixBox[4][4].pozY, "="), offset = offset + textwidth("=") + 10;
        }
    }

    s = 0;
    for (i=1; i<=countLin; i++)
        s += MatrixBox[i][i].value;
    NumbertoChar(s, functionCall_v);
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
    settextjustify(LEFT_TEXT, TOP_TEXT);
    outtextxy(offset, MatrixBox[4][4].pozY, functionCall_v);
    delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
}

void SumDiagP_U() //suma de sub diagonala principala
{
    bar(MatrixBox[4][4].pozX + MatrixBox[4][4].width + 10, MatrixBox[4][4].pozY - 4, screenWidth, MatrixBox[4][4].pozY + MatrixBox[4][4].height);
    functionCall_v[0] = '\0';
    int i, j, offset = MatrixBox[4][4].pozX + MatrixBox[4][4].width + 20, s;

    for (int k = 1; k<=countLin; k++)
    {
        for (j=1; j<k; j++)
        {
            animateMbox(k, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
            setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
            settextstyle(settings.textFont, HORIZ_DIR, 3);
            settextjustify(LEFT_TEXT, TOP_TEXT);
            outtextxy(offset, MatrixBox[4][4].pozY, MatrixBox[k][j].text);
            delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
            offset = offset + textwidth(MatrixBox[k][j].text) + 10;
            if (k == countLin && j == countLin-1)
            {
                if(settings.playFXSounds)
                    PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);
                outtextxy(offset, MatrixBox[4][4].pozY, "="), offset = offset + textwidth("=") + 10;
            }
            else
                outtextxy(offset, MatrixBox[4][4].pozY, "+"), offset = offset + textwidth("+") + 10;
        }
    }

    s = 0;
    for (i=1; i<=countLin; i++)
        for (j=1; j<i; j++)
            if (j<i)
                s += MatrixBox[i][j].value;

    NumbertoChar(s, functionCall_v);
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
    settextjustify(LEFT_TEXT, TOP_TEXT);
    outtextxy(offset, MatrixBox[4][4].pozY, functionCall_v);
    delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
}

void SumDiagP_A() //suma de deasupra diagonalei principale
{
    bar(MatrixBox[4][4].pozX + MatrixBox[4][4].width + 10, MatrixBox[4][4].pozY - 4, screenWidth, MatrixBox[4][4].pozY + MatrixBox[4][4].height);
    functionCall_v[0] = '\0';
    int i, j, offset = MatrixBox[4][4].pozX + MatrixBox[4][4].width + 20, s;

    for (int k = 1; k<=countLin; k++)
    {
        for (j=k+1; j<=countCol; j++)
        {
            animateMbox(k, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
            setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
            settextstyle(settings.textFont, HORIZ_DIR, 3);
            settextjustify(LEFT_TEXT, TOP_TEXT);
            outtextxy(offset, MatrixBox[4][4].pozY, MatrixBox[k][j].text);
            delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
            offset = offset + textwidth(MatrixBox[k][j].text) + 10;
            if (k == countLin-1 && j == countLin)
            {
                if(settings.playFXSounds)
                    PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);
                outtextxy(offset, MatrixBox[4][4].pozY, "="), offset = offset + textwidth("=") + 10;
            }
            else
                outtextxy(offset, MatrixBox[4][4].pozY, "+"), offset = offset + textwidth("+") + 10;
        }
    }

    s = 0;
    for (i=1; i<=countLin; i++)
        for (j=countCol; j>i; j--)
            s += MatrixBox[i][j].value;

    NumbertoChar(s, functionCall_v);
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
    settextjustify(LEFT_TEXT, TOP_TEXT);
    outtextxy(offset, MatrixBox[4][4].pozY, functionCall_v);
    delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
}

void SumDiagS()
{
    bar(MatrixBox[4][4].pozX + MatrixBox[4][4].width + 10, MatrixBox[4][4].pozY - 4, screenWidth, MatrixBox[4][4].pozY + MatrixBox[4][4].height);
    functionCall_v[0] = '\0';
    int i, j, offset = MatrixBox[4][4].pozX + MatrixBox[4][4].width + 20, s;

    for (int k = 1; k<=countLin; k++)
    {
        for (j=1; j<=countCol; j++)
            if(k+j == countLin + 1)
            {
                animateMbox(k, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
                settextstyle(settings.textFont, HORIZ_DIR, 3);
                settextjustify(LEFT_TEXT, TOP_TEXT);
                outtextxy(offset, MatrixBox[4][4].pozY, MatrixBox[k][j].text);
                delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
                offset = offset + textwidth(MatrixBox[k][j].text) + 10;
                if (k == countLin && j == 1)
                {
                    if(settings.playFXSounds)
                        PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);
                    outtextxy(offset, MatrixBox[4][4].pozY, "="), offset = offset + textwidth("=") + 10;
                }
                else
                    outtextxy(offset, MatrixBox[4][4].pozY, "+"), offset = offset + textwidth("+") + 10;
            }
    }

    s = 0;
    for (i = 1; i<=countLin; i++)
        for (j=1; j<=countCol; j++)
            if(i+j == countLin + 1)
                s += MatrixBox[i][j].value;

    NumbertoChar(s, functionCall_v);
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
    settextjustify(LEFT_TEXT, TOP_TEXT);
    outtextxy(offset, MatrixBox[4][4].pozY, functionCall_v);
    delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
}

void SumDiagS_A()
{
    bar(MatrixBox[4][4].pozX + MatrixBox[4][4].width + 10, MatrixBox[4][4].pozY - 4, screenWidth, MatrixBox[4][4].pozY + MatrixBox[4][4].height);
    functionCall_v[0] = '\0';
    int i, j, offset = MatrixBox[4][4].pozX + MatrixBox[4][4].width + 20, s;

    for (int k = 1; k<=countLin; k++)
    {
        for (j=1; j<=countCol; j++)
            if(k+j < countLin + 1)
            {
                animateMbox(k, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
                settextstyle(settings.textFont, HORIZ_DIR, 3);
                settextjustify(LEFT_TEXT, TOP_TEXT);
                outtextxy(offset, MatrixBox[4][4].pozY, MatrixBox[k][j].text);
                delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
                offset = offset + textwidth(MatrixBox[k][j].text) + 10;
                if (k == countLin - 1 && j == 1)
                {
                    if(settings.playFXSounds)
                        PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);
                    outtextxy(offset, MatrixBox[4][4].pozY, "="), offset = offset + textwidth("=") + 10;
                }
                else
                    outtextxy(offset, MatrixBox[4][4].pozY, "+"), offset = offset + textwidth("+") + 10;
            }
    }

    s = 0;
    for (i = 1; i<=countLin; i++)
        for (j=1; j<=countCol; j++)
            if(i+j < countLin + 1)
                s += MatrixBox[i][j].value;

    NumbertoChar(s, functionCall_v);
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
    settextjustify(LEFT_TEXT, TOP_TEXT);
    outtextxy(offset, MatrixBox[4][4].pozY, functionCall_v);
    delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
}

void SumDiagS_U()
{
    bar(MatrixBox[4][4].pozX + MatrixBox[4][4].width + 10, MatrixBox[4][4].pozY - 4, screenWidth, MatrixBox[4][4].pozY + MatrixBox[4][4].height);
    functionCall_v[0] = '\0';
    int i, j, offset = MatrixBox[4][4].pozX + MatrixBox[4][4].width + 20, s;

    for (int k = 1; k<=countLin; k++)
    {
        for (j=1; j<=countCol; j++)
            if(k+j > countLin + 1)
            {
                animateMbox(k, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
                settextstyle(settings.textFont, HORIZ_DIR, 3);
                settextjustify(LEFT_TEXT, TOP_TEXT);
                outtextxy(offset, MatrixBox[4][4].pozY, MatrixBox[k][j].text);
                delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
                offset = offset + textwidth(MatrixBox[k][j].text) + 10;
                if (k == countLin && j == countLin)
                {
                    if(settings.playFXSounds)
                        PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);
                    outtextxy(offset, MatrixBox[4][4].pozY, "="), offset = offset + textwidth("=") + 10;
                }
                else
                    outtextxy(offset, MatrixBox[4][4].pozY, "+"), offset = offset + textwidth("+") + 10;
            }
    }

    s = 0;
    for (i = 1; i<=countLin; i++)
        for (j=1; j<=countCol; j++)
            if(i+j > countLin + 1)
                s += MatrixBox[i][j].value;

    NumbertoChar(s, functionCall_v);
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
    settextjustify(LEFT_TEXT, TOP_TEXT);
    outtextxy(offset, MatrixBox[4][4].pozY, functionCall_v);
    delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
}

///urmatoarele functii creeaza si afiseaza fiecare buton

void showButtonVector() //afiseasa si creeaza butonul din pagina de start pentru vectori
{
    ButtonVector.height = 70;
    ButtonVector.width = 300;
    ButtonVector.pozX = screenWidth/8 - ButtonVector.width/2;
    ButtonVector.pozY = 250;
    drawButton(ButtonVector);
    strcpy(ButtonVector.text.buttonText, "Vectori");
    settextstyle(settings.textFont, HORIZ_DIR, 5);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    outtextxy(ButtonVector.pozX + ButtonVector.width/2, ButtonVector.pozY + ButtonVector.height/2 + 10, "Vectori");
    ButtonVector.show = true;
}

void showButtonMatrix() //afiseaza si creeaza butonul din pagina de start pentru matrici
{
    ButtonMatrix.height = 70;
    ButtonMatrix.width = 300;
    ButtonMatrix.pozX = screenWidth/8 - ButtonMatrix.width/2;
    ButtonMatrix.pozY = 350;
    drawButton(ButtonMatrix);
    strcpy(ButtonMatrix.text.buttonText, "Matrici");
    settextstyle(settings.textFont, HORIZ_DIR, 5);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    outtextxy(ButtonMatrix.pozX + ButtonMatrix.width/2, ButtonMatrix.pozY + ButtonMatrix.height/2 + 10, "Matrici");
    ButtonMatrix.show = true;
}

void showButtonSettings() //afiseaza si creeaza butonul din pagina de start pentru matrici
{
    ButtonSettings.height = 70;
    ButtonSettings.width = 300;
    ButtonSettings.pozX = screenWidth/8 - ButtonSettings.width/2;
    ButtonSettings.pozY = 650;
    drawButton(ButtonSettings);
    strcpy(ButtonSettings.text.buttonText, "Setari");
    settextstyle(settings.textFont, HORIZ_DIR, 5);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    outtextxy(ButtonSettings.pozX + ButtonSettings.width/2, ButtonSettings.pozY + ButtonSettings.height/2 + 10, "Setari");
    ButtonSettings.show = true;
}

void showButtonInstructions()
{
    ButtonInstructions.height = 70;
    ButtonInstructions.width = 300;
    ButtonInstructions.pozX = screenWidth/8 - ButtonInstructions.width/2;
    ButtonInstructions.pozY = 550;
    drawButton(ButtonInstructions);
    strcpy(ButtonInstructions.text.buttonText, "Instructiuni");
    settextstyle(settings.textFont, HORIZ_DIR, 5);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    outtextxy(ButtonInstructions.pozX + ButtonInstructions.width/2, ButtonInstructions.pozY + ButtonInstructions.height/2 + 10, "Instructiuni");
    ButtonInstructions.show = true;
}

void showButtonExit()
{
    ButtonExit.height = 50;
    ButtonExit.width = 200;
    ButtonExit.pozX = screenWidth/8 - ButtonExit.width/2;
    ButtonExit.pozY = 750;
    drawButton(ButtonExit);
    strcpy(ButtonExit.text.buttonText, "Iesire");
    settextstyle(settings.textFont, HORIZ_DIR, 4);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    outtextxy(ButtonExit.pozX + ButtonExit.width/2, ButtonExit.pozY + ButtonExit.height/2 + 10, "Iesire");
    ButtonExit.show = true;
}

void showButtonSum_V()
{
    ButtonSum_V.height = 40;
    ButtonSum_V.width = 300;
    ButtonSum_V.pozX = screenWidth/3 + 60 - ButtonSum_V.width/2;
    ButtonSum_V.pozY = 300;
    drawButton(ButtonSum_V);
    strcpy(ButtonSum_V.text.buttonText, "Suma elementelor");
    settextstyle(settings.textFont, HORIZ_DIR, 2);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    outtextxy(ButtonSum_V.pozX + ButtonSum_V.width/2, ButtonSum_V.pozY + ButtonSum_V.height/2 + 5, "Suma elementelor");
    ButtonSum_V.show = true;
}

void showButtonMult_V()
{
    ButtonMult_V.height = 40;
    ButtonMult_V.width = 300;
    ButtonMult_V.pozX = screenWidth/3 + 60 - ButtonMult_V.width/2;
    ButtonMult_V.pozY = 350;
    drawButton(ButtonMult_V);
    strcpy(ButtonMult_V.text.buttonText, "Produsul elementelor");
    settextstyle(settings.textFont, HORIZ_DIR, 2);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    outtextxy(ButtonMult_V.pozX + ButtonMult_V.width/2, ButtonMult_V.pozY + ButtonMult_V.height/2 + 6, "Produsul elementelor");
    ButtonMult_V.show = true;
}

void showButtonScalar_V()
{
    ButtonScalar_V.height = 40;
    ButtonScalar_V.width = 300;
    ButtonScalar_V.pozX = screenWidth/3 + 60 - ButtonScalar_V.width/2;
    ButtonScalar_V.pozY = 400;
    drawButton(ButtonScalar_V);
    strcpy(ButtonScalar_V.text.buttonText, "Inmultirea cu un scalar");
    settextstyle(settings.textFont, HORIZ_DIR, 2);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    outtextxy(ButtonScalar_V.pozX + ButtonScalar_V.width/2, ButtonScalar_V.pozY + ButtonScalar_V.height/2 + 6, "Inmultirea cu un numar");
    ButtonScalar_V.show = true;
}

void showButtonBubbleSort_V()
{
    ButtonBubbleSort_V.height = 40;
    ButtonBubbleSort_V.width = 300;
    ButtonBubbleSort_V.pozX = screenWidth/3 + 60 - ButtonBubbleSort_V.width/2;
    ButtonBubbleSort_V.pozY = 450;
    drawButton(ButtonBubbleSort_V);
    strcpy(ButtonBubbleSort_V.text.buttonText, "BubbleSort");
    settextstyle(settings.textFont, HORIZ_DIR, 2);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    outtextxy(ButtonBubbleSort_V.pozX + ButtonBubbleSort_V.width/2, ButtonBubbleSort_V.pozY + ButtonBubbleSort_V.height/2 + 6, "BubbleSort");
    ButtonBubbleSort_V.show = true;
}

void showButtonQuickSort_V()
{
    ButtonQuickSort_V.height = 40;
    ButtonQuickSort_V.width = 300;
    ButtonQuickSort_V.pozX = ButtonBubbleSort_V.pozX + ButtonBubbleSort_V.width + 15;
    ButtonQuickSort_V.pozY = 450;
    drawButton(ButtonQuickSort_V);
    strcpy(ButtonQuickSort_V.text.buttonText, "QuickSort");
    settextstyle(settings.textFont, HORIZ_DIR, 2);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    outtextxy(ButtonQuickSort_V.pozX + ButtonQuickSort_V.width/2, ButtonQuickSort_V.pozY + ButtonQuickSort_V.height/2 + 6, "QuickSort");
    ButtonQuickSort_V.show = true;
}

void showButtonShift_R()
{
    ButtonShift_R.height = 40;
    ButtonShift_R.width = 400;
    ButtonShift_R.pozX = screenWidth/3 + 60 - ButtonSum_V.width/2;
    ButtonShift_R.pozY = 500;
    drawButton(ButtonShift_R);
    strcpy(ButtonShift_R.text.buttonText, "Shiftare la dreapta");
    settextstyle(settings.textFont, HORIZ_DIR, 2);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    outtextxy(ButtonShift_R.pozX + ButtonShift_R.width/2, ButtonShift_R.pozY + ButtonShift_R.height/2 + 6, "Shiftare circulara la dreapta");
    ButtonShift_R.show = true;
}

void showButtonShift_L()
{
    ButtonShift_L.height = 40;
    ButtonShift_L.width = 400;
    ButtonShift_L.pozX = ButtonShift_R.pozX + ButtonShift_R.width + 15;
    ButtonShift_L.pozY = 500;
    drawButton(ButtonShift_L);
    strcpy(ButtonShift_L.text.buttonText, "Shiftare la stanga");
    settextstyle(settings.textFont, HORIZ_DIR, 2);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    outtextxy(ButtonShift_L.pozX + ButtonShift_L.width/2, ButtonShift_L.pozY + ButtonShift_L.height/2 + 6, "Shiftare circulara la stanga");
    ButtonShift_L.show = true;
}

void showButtonElim_V()
{
    ButtonElim_V.height = 40;
    ButtonElim_V.width = 500;
    ButtonElim_V.pozX = screenWidth/3 + 60 - ButtonSum_V.width/2;
    ButtonElim_V.pozY = 550;
    drawButton(ButtonElim_V);
    strcpy(ButtonElim_V.text.buttonText, "Elimina un element");
    settextstyle(settings.textFont, HORIZ_DIR, 2);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    outtextxy(ButtonElim_V.pozX + ButtonElim_V.width/2, ButtonElim_V.pozY + ButtonElim_V.height/2 + 6, "Elimina elementul de pe pozitia:");
    ButtonElim_V.show = true;
}

void showButtonSavedFile_V()
{
    ButtonSavedFile_V.height = 40;
    ButtonSavedFile_V.width = 300;
    ButtonSavedFile_V.pozX = screenWidth/3 + 60 - ButtonSavedFile_V.width/2;
    ButtonSavedFile_V.pozY = 700;
    drawButton(ButtonSavedFile_V);
    strcpy(ButtonSavedFile_V.text.buttonText, "Incarca ultima salvare");
    settextstyle(settings.textFont, HORIZ_DIR, 2);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    outtextxy(ButtonSavedFile_V.pozX + ButtonSavedFile_V.width/2, ButtonSavedFile_V.pozY + ButtonSavedFile_V.height/2 + 6, "Incarca ultima salvare");
    ButtonSavedFile_V.show = true;
}

void showButtonNLin_M()
{
    ButtonNLin_M.height = 40;
    ButtonNLin_M.width = 270;
    ButtonNLin_M.pozX = MatrixBox[1][1].pozX;
    ButtonNLin_M.pozY = MatrixBox[4][4].pozY + MatrixBox[4][4].height + 25;
    drawButton(ButtonNLin_M);
    strcpy(ButtonNLin_M.text.buttonText, "Numarul de linii");
    settextstyle(settings.textFont, HORIZ_DIR, 2);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    outtextxy(ButtonNLin_M.pozX + ButtonNLin_M.width/2, ButtonNLin_M.pozY + ButtonNLin_M.height/2 + 6, "Numarul de linii:");
    ButtonNLin_M.show = true;
}

void showButtonNCol_M()
{
    ButtonNCol_M.height = 40;
    ButtonNCol_M.width = 270;
    ButtonNCol_M.pozX = MatrixBox[1][1].pozX;
    ButtonNCol_M.pozY = ButtonNLin_M.pozY + ButtonNLin_M.height + 10;
    drawButton(ButtonNCol_M);
    strcpy(ButtonNCol_M.text.buttonText, "Numarul de coloane");
    settextstyle(settings.textFont, HORIZ_DIR, 2);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    outtextxy(ButtonNCol_M.pozX + ButtonNCol_M.width/2, ButtonNCol_M.pozY + ButtonNCol_M.height/2 + 6, "Numarul de coloane:");
    ButtonNCol_M.show = true;
}

void showButtonSum_M()
{
    ButtonSum_M.height = 40;
    ButtonSum_M.width = 270;
    ButtonSum_M.pozX = MatrixBox[1][1].pozX;
    ButtonSum_M.pozY = ButtonNCol_M.pozY + ButtonNCol_M.height + 10;
    drawButton(ButtonSum_M);
    strcpy(ButtonSum_M.text.buttonText, "Suma");
    settextstyle(settings.textFont, HORIZ_DIR, 2);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    int prev = getcolor();
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
    outtextxy(ButtonSum_M.pozX + ButtonSum_M.width/2, ButtonSum_M.pozY + ButtonSum_M.height/2 + 6, "Suma cu o matrice");
    setcolor(prev);
    ButtonSum_M.show = true;
}

void showButtonMult_M()
{
    ButtonMult_M.height = 40;
    ButtonMult_M.width = 300;
    ButtonMult_M.pozX = ButtonSum_M.pozX + ButtonSum_M.width + 10;
    ButtonMult_M.pozY = ButtonNCol_M.pozY + ButtonNCol_M.height + 10;
    drawButton(ButtonMult_M);
    strcpy(ButtonMult_M.text.buttonText, "Inmultire");
    settextstyle(settings.textFont, HORIZ_DIR, 2);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    int prev = getcolor();
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
    outtextxy(ButtonMult_M.pozX + ButtonMult_M.width/2, ButtonMult_M.pozY + ButtonMult_M.height/2 + 6, "Produsul cu o matrice");
    setcolor(prev);
    ButtonMult_M.show = true;
}

void showButtonPower_M()
{
    ButtonPower_M.height = 40;
    ButtonPower_M.width = 300;
    ButtonPower_M.pozX = ButtonMult_M.pozX + ButtonMult_M.width + 10;
    ButtonPower_M.pozY = ButtonNCol_M.pozY + ButtonNCol_M.height + 10;
    drawButton(ButtonPower_M);
    strcpy(ButtonPower_M.text.buttonText, "Putere");
    settextstyle(settings.textFont, HORIZ_DIR, 2);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    int prev = getcolor();
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
    outtextxy(ButtonPower_M.pozX + ButtonPower_M.width/2, ButtonPower_M.pozY + ButtonPower_M.height/2 + 6, "Ridicare la putere");
    setcolor(prev);
    ButtonPower_M.show = true;
}

void showButtonDet_M()
{
    ButtonDet_M.height = 40;
    ButtonDet_M.width = 270;
    ButtonDet_M.pozX = MatrixBox[1][1].pozX;
    ButtonDet_M.pozY = ButtonSum_M.pozY + ButtonSum_M.height + 10;
    drawButton(ButtonDet_M);
    strcpy(ButtonDet_M.text.buttonText, "Determinantul");
    settextstyle(settings.textFont, HORIZ_DIR, 2);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    int prev = getcolor();
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
    outtextxy(ButtonDet_M.pozX + ButtonDet_M.width/2, ButtonDet_M.pozY + ButtonDet_M.height/2 + 6, "Determinantul");
    setcolor(prev);
    ButtonDet_M.show = true;
}

void showButtonTrans_M()
{
    ButtonTrans_M.height = 40;
    ButtonTrans_M.width = 300;
    ButtonTrans_M.pozX = ButtonDet_M.pozX + ButtonDet_M.width + 10;
    ButtonTrans_M.pozY = ButtonSum_M.pozY + ButtonSum_M.height + 10;
    drawButton(ButtonTrans_M);
    strcpy(ButtonTrans_M.text.buttonText, "Transpusa");
    settextstyle(settings.textFont, HORIZ_DIR, 2);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    int prev = getcolor();
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
    outtextxy(ButtonTrans_M.pozX + ButtonTrans_M.width/2, ButtonTrans_M.pozY + ButtonTrans_M.height/2 + 6, "Transpusa");
    setcolor(prev);
    ButtonTrans_M.show = true;
}

void showButtonInv_M()
{
    ButtonInv_M.height = 40;
    ButtonInv_M.width = 300;
    ButtonInv_M.pozX = ButtonTrans_M.pozX + ButtonTrans_M.width + 10;
    ButtonInv_M.pozY = ButtonSum_M.pozY + ButtonSum_M.height + 10;
    drawButton(ButtonInv_M);
    strcpy(ButtonInv_M.text.buttonText, "Inversa");
    settextstyle(settings.textFont, HORIZ_DIR, 2);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    int prev = getcolor();
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
    outtextxy(ButtonInv_M.pozX + ButtonInv_M.width/2, ButtonInv_M.pozY + ButtonInv_M.height/2 + 6, "Inversa");
    setcolor(prev);
    ButtonInv_M.show = true;
}

void showButtonSumElem_M()
{
    ButtonSumElem_M.height = 40;
    ButtonSumElem_M.width = 270;
    ButtonSumElem_M.pozX = MatrixBox[1][1].pozX;
    ButtonSumElem_M.pozY = ButtonDet_M.pozY + ButtonDet_M.height + 10;
    drawButton(ButtonSumElem_M);
    strcpy(ButtonSumElem_M.text.buttonText, "Suma elementelor");
    settextstyle(settings.textFont, HORIZ_DIR, 2);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    int prev = getcolor();
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
    outtextxy(ButtonSumElem_M.pozX + ButtonSumElem_M.width/2, ButtonSumElem_M.pozY + ButtonSumElem_M.height/2 + 6, "Suma elementelor");
    setcolor(prev);
    ButtonSumElem_M.show = true;
}

void showButtonSums()
{
    ButtonSumDiagP.height = 40;
    ButtonSumDiagP.width = 300;
    ButtonSumDiagP.pozX = MatrixBox[4][4].pozX + MatrixBox[4][4].width + 20;
    ButtonSumDiagP.pozY = MatrixBox[1][4].pozY;
    drawButton(ButtonSumDiagP);
    strcpy(ButtonSumDiagP.text.buttonText, "Diag P");
    settextstyle(settings.textFont, HORIZ_DIR, 1);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    int prev = getcolor();
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
    outtextxy(ButtonSumDiagP.pozX + ButtonSumDiagP.width/2, ButtonSumDiagP.pozY + ButtonSumDiagP.height/2 + 6, "De pe diag. principala");
    setcolor(prev);
    ButtonSumDiagP.show = true;

    ButtonSumDiagP_U.height = 40;
    ButtonSumDiagP_U.width = 300;
    ButtonSumDiagP_U.pozX = MatrixBox[4][4].pozX + MatrixBox[4][4].width + 20;
    ButtonSumDiagP_U.pozY = ButtonSumDiagP.pozY + ButtonSumDiagP.height + 10;
    drawButton(ButtonSumDiagP_U);
    strcpy(ButtonSumDiagP_U.text.buttonText, "Diag P");
    settextstyle(settings.textFont, HORIZ_DIR, 1);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    prev = getcolor();
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
    outtextxy(ButtonSumDiagP_U.pozX + ButtonSumDiagP_U.width/2, ButtonSumDiagP_U.pozY + ButtonSumDiagP_U.height/2 + 6, "De sub diag. principala");
    setcolor(prev);
    ButtonSumDiagP_U.show = true;

    ButtonSumDiagP_A.height = 40;
    ButtonSumDiagP_A.width = 300;
    ButtonSumDiagP_A.pozX = MatrixBox[4][4].pozX + MatrixBox[4][4].width + 20;
    ButtonSumDiagP_A.pozY = ButtonSumDiagP_U.pozY + ButtonSumDiagP_U.height + 10;
    drawButton(ButtonSumDiagP_A);
    strcpy(ButtonSumDiagP_A.text.buttonText, "Diag P");
    settextstyle(settings.textFont, HORIZ_DIR, 1);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    prev = getcolor();
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
    outtextxy(ButtonSumDiagP_A.pozX + ButtonSumDiagP_A.width/2, ButtonSumDiagP_A.pozY + ButtonSumDiagP_A.height/2 + 6, "De deasupra diag. principale");
    setcolor(prev);
    ButtonSumDiagP_A.show = true;

    ButtonSumDiagS.height = 40;
    ButtonSumDiagS.width = 300;
    ButtonSumDiagS.pozX = ButtonSumDiagP.pozX + ButtonSumDiagP.width + 15;
    ButtonSumDiagS.pozY = MatrixBox[1][4].pozY;
    drawButton(ButtonSumDiagS);
    strcpy(ButtonSumDiagS.text.buttonText, "Diag S");
    settextstyle(settings.textFont, HORIZ_DIR, 1);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    prev = getcolor();
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
    outtextxy(ButtonSumDiagS.pozX + ButtonSumDiagS.width/2, ButtonSumDiagS.pozY + ButtonSumDiagS.height/2 + 6, "De pe diag. secundara");
    setcolor(prev);
    ButtonSumDiagS.show = true;

    ButtonSumDiagS_U.height = 40;
    ButtonSumDiagS_U.width = 300;
    ButtonSumDiagS_U.pozX =  ButtonSumDiagP.pozX + ButtonSumDiagP.width + 15;
    ButtonSumDiagS_U.pozY = ButtonSumDiagS.pozY + ButtonSumDiagS.height + 10;
    drawButton(ButtonSumDiagS_U);
    strcpy(ButtonSumDiagS_U.text.buttonText, "Diag S");
    settextstyle(settings.textFont, HORIZ_DIR, 1);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    prev = getcolor();
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
    outtextxy(ButtonSumDiagS_U.pozX + ButtonSumDiagS_U.width/2, ButtonSumDiagS_U.pozY +ButtonSumDiagS_U.height/2 + 6, "De sub diag. secundara");
    setcolor(prev);
    ButtonSumDiagS_U.show = true;

    ButtonSumDiagS_A.height = 40;
    ButtonSumDiagS_A.width = 300;
    ButtonSumDiagS_A.pozX =  ButtonSumDiagP.pozX + ButtonSumDiagP.width + 15;
    ButtonSumDiagS_A.pozY = ButtonSumDiagS_U.pozY + ButtonSumDiagS_U.height + 10;
    drawButton(ButtonSumDiagS_A);
    strcpy(ButtonSumDiagS_A.text.buttonText, "Diag S");
    settextstyle(settings.textFont, HORIZ_DIR, 1);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    prev = getcolor();
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
    outtextxy(ButtonSumDiagS_A.pozX + ButtonSumDiagS_A.width/2, ButtonSumDiagS_A.pozY + ButtonSumDiagS_A.height/2 + 6, "De deasupra diag. secundare");
    setcolor(prev);
    ButtonSumDiagS_A.show = true;
}

void showButtonSavedFile_M()
{
    ButtonSavedFile_M.height = 40;
    ButtonSavedFile_M.width = 300;
    ButtonSavedFile_M.pozX = ButtonSum_M.pozX;
    ButtonSavedFile_M.pozY = 700;
    drawButton(ButtonSavedFile_M);
    strcpy(ButtonSavedFile_M.text.buttonText, "Incarca ultima salvare");
    settextstyle(settings.textFont, HORIZ_DIR, 2);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    outtextxy(ButtonSavedFile_M.pozX + ButtonSavedFile_M.width/2, ButtonSavedFile_M.pozY + ButtonSavedFile_M.height/2 + 6, "Incarca ultima salvare");
    ButtonSavedFile_M.show = true;
}

void showButtonDarkTheme()
{
    ButtonDarkTheme.height = 40;
    ButtonDarkTheme.width = 300;
    ButtonDarkTheme.pozX = 500;
    ButtonDarkTheme.pozY = 300;
    drawButton(ButtonDarkTheme);
    strcpy(ButtonDarkTheme.text.buttonText, "Dark Theme");
    settextstyle(settings.textFont, HORIZ_DIR, 2);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    outtextxy(ButtonDarkTheme.pozX + ButtonDarkTheme.width/2, ButtonDarkTheme.pozY + ButtonDarkTheme.height/2 + 6, "Tema intunecata");
    ButtonDarkTheme.show = true;
}

void showButtonMute()
{
    ButtonMute.height = 40;
    ButtonMute.width = 300;
    ButtonMute.pozX = 500;
    ButtonMute.pozY = 400;
    drawButton(ButtonMute);
    strcpy(ButtonMute.text.buttonText, "Mute");
    settextstyle(settings.textFont, HORIZ_DIR, 2);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    outtextxy(ButtonMute.pozX + ButtonMute.width/2, ButtonMute.pozY + ButtonMute.height/2 + 6, "Fara sunete");
    ButtonMute.show = true;
}

void showButtonSlowAnim()
{
    ButtonSlowAnim.height = 40;
    ButtonSlowAnim.width = 300;
    ButtonSlowAnim.pozX = 500;
    ButtonSlowAnim.pozY = 500;
    drawButton(ButtonSlowAnim);
    strcpy(ButtonSlowAnim.text.buttonText, "Slow Animations");
    settextstyle(settings.textFont, HORIZ_DIR, 2);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    outtextxy(ButtonSlowAnim.pozX + ButtonSlowAnim.width/2, ButtonSlowAnim.pozY + ButtonSlowAnim.height/2 + 6, "Animatii lente");
    ButtonSlowAnim.show = true;
}

void showStartButtons()
{
    showButtonVector();
    showButtonMatrix();
    showButtonInstructions();
    showButtonSettings();
    showButtonExit();
}

void VectorButtonAnimation()
{
    setcolor(settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
    drawButton(ButtonVector);
}

void ExitVectorButtonAnimation()
{
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
    drawButton(ButtonVector);
}

void MatrixButtonAnimation()
{
    setcolor(settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
    drawButton(ButtonMatrix);
}

void ExitMatrixButtonAnimation()
{
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
    drawButton(ButtonMatrix);
}

void InstructionsButtonAnimation()
{
    setcolor(settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
    drawButton(ButtonInstructions);
}

void ExitInstructionsButtonAnimation()
{
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
    drawButton(ButtonInstructions);
}

void SettingsButtonAnimation()
{
    setcolor(settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
    drawButton(ButtonSettings);
}

void ExitSettingsButtonAnimation()
{
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
    drawButton(ButtonSettings);
}

void ExitButtonAnimation()
{
    setcolor(settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
    drawButton(ButtonExit);
}

void ExitExitButtonAnimation()
{
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
    drawButton(ButtonExit);
}

int RequestNumber_V(Button b) //se utilizeaza atunci cand prin apasarea unui buton se cere introducerea unui numar
{
    bool negative = false;
    bar(b.pozX + b.width + 4, b.pozY - 1, screenWidth, b.pozY + b.height);
    settextjustify(LEFT_TEXT, CENTER_TEXT);
    char t[20], c;
    int poz = 0, num = 0;

    while (!kbhit())
    {
        outtextxy(b.pozX + b.width + 20, b.pozY + 20, "_");
        delay(400);
        bar(b.pozX + b.width + 4, b.pozY - 1, screenWidth, b.pozY + b.height);
        delay(300);
    }

    c = getch();
    while(c!=13)
    {
        bar(b.pozX + b.width + 4, b.pozY - 1, screenWidth, b.pozY + b.height);
        if (c==45 && poz==0)
        {
            negative = true;
            t[poz] = c;
            poz++;
            t[poz] = '\0';
            outtextxy(b.pozX + b.width + 20, b.pozY + 20, t);
        }
        else
        {
            if (c!=8 && isdigit(c))
            {
                t[poz] = c;
                poz++;
                t[poz] = '\0';
                num = num*10 + (int)c - 48;
                outtextxy(b.pozX + b.width + 20, b.pozY + 20, t);
            }
            else if (poz)
            {
                poz--;
                t[poz] = '\0';
                num = num/10;
                outtextxy(b.pozX + b.width + 20, b.pozY + 20, t);
            }
            else
            {
                t[poz] = '\0';
            }
        }

        c = getch();
    }

    if(negative)
    {
        num = ChartoNumber(t);
    }
    t[poz] = '\0';

    return num;
}

void showAllVbox(int pos) //afisarea intregului vector pornind cu pozitia pos
{
    int i = pos;

    bar(VectorBox[i].pozX, VectorBox[i].pozY - 4, screenWidth, VectorBox[i].pozY + VectorBox[i].height + 4);

    for (i = pos; i<=countV+1; i++)
    {
        if(VectorBox[i].show == 1)
        {
            if(i!=1)
                VectorBox[i].pozX = VectorBox[i-1].pozX + VectorBox[i-1].width + 10;

            rectangle(VectorBox[i].pozX, VectorBox[i].pozY, VectorBox[i].pozX + VectorBox[i].width, VectorBox[i].pozY + VectorBox[i].height);
            settextjustify(CENTER_TEXT, CENTER_TEXT);
            outtextxy(VectorBox[i].pozX + VectorBox[i].width/2 + 1, VectorBox[i].pozY + VectorBox[i].height/2 + 5, VectorBox[i].text);
        }
    }
}

void StartScreen()
{
    setbkcolor(settings.darkTheme?settings.bkColor.dark:settings.bkColor.light); //fundalul
    setfillstyle(1, settings.darkTheme?settings.bkColor.dark:settings.bkColor.light); //culoare pentru functia bar
    cleardevice(); //pentru a seta culoarea de fundal

    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light); //culoarea textului
    setlinestyle(0, 0, 3);
    settextstyle(settings.textFont, HORIZ_DIR, 7);
    settextjustify(CENTER_TEXT, TOP_TEXT);

    showStartButtons();
    if (settings.darkTheme)
        readimagefile("logo_dark.jpg", 15, 60, 365, 189);
    else
        readimagefile("logo_light.jpg", 15, 60, 365, 189);

    line(ButtonVector.width + 2*ButtonVector.pozX + 10, 1, ButtonVector.width + 2*ButtonVector.pozX + 10, screenHeight);
    if(startSettings)
        SettingsScreen();
}

void createVectorBox(char element[], int PosVectorBox) //cream casuta elementelui din vector
{
    settextstyle(settings.textFont, HORIZ_DIR, 3);
    settextjustify(CENTER_TEXT, CENTER_TEXT);

    VectorBox[PosVectorBox].width = textwidth(element) + 20;
    strcpy(VectorBox[PosVectorBox].text, element);

    showAllVbox(PosVectorBox);
}

void showAllMbox(int LinMatrixBox, int ColMatrixBox)
{
    bar(MatrixBox[LinMatrixBox][LinMatrixBox].pozX, MatrixBox[LinMatrixBox][LinMatrixBox].pozY - 4, MatrixBox[4][4].pozX + MatrixBox[4][4].width + 4, MatrixBox[4][4].pozY + MatrixBox[4][4].height + 4);

    for (int i = LinMatrixBox; i<=4; i++)
    {
        for (int j = 1; j<=4; j++)
        {
            if (i<=countLin && j<=countCol)
                MatrixBox[i][j].color = LIGHTRED, setcolor(MatrixBox[i][j].color);
            else
                MatrixBox[i][j].color = 1;
            rectangle(MatrixBox[i][j].pozX, MatrixBox[i][j].pozY, MatrixBox[i][j].pozX + MatrixBox[i][j].width, MatrixBox[i][j].pozY + MatrixBox[i][j].height);
            setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
            settextjustify(CENTER_TEXT, CENTER_TEXT);
            outtextxy(MatrixBox[i][j].pozX + MatrixBox[i][j].width/2 + 1, MatrixBox[i][j].pozY + MatrixBox[i][j].height/2 + 5, MatrixBox[i][j].text);
        }
    }
}

void showAllMbox2(int LinMatrixBox, int ColMatrixBox)
{
    bar(MatrixBox2[LinMatrixBox][LinMatrixBox].pozX, MatrixBox2[LinMatrixBox][LinMatrixBox].pozY - 4, MatrixBox2[4][4].pozX + MatrixBox2[4][4].width + 4, MatrixBox2[4][4].pozY + MatrixBox2[4][4].height + 4);

    for (int i = LinMatrixBox; i<=4; i++)
    {
        for (int j = 1; j<=4; j++)
        {
            if (showMatrixResultMult)
            {
                if (i<=countCol && j<=countCol2)
                    MatrixBox2[i][j].color = LIGHTRED, setcolor(MatrixBox2[i][j].color);
                else
                    MatrixBox2[i][j].color = settings.darkTheme?settings.textColor.dark:settings.textColor.light;
            }
            else
            {
                if (i<=countLin && j<=countCol)
                    MatrixBox2[i][j].color = LIGHTRED, setcolor(MatrixBox2[i][j].color);
                else
                    MatrixBox2[i][j].color = settings.darkTheme?settings.textColor.dark:settings.textColor.light;
            }

            setcolor(MatrixBox2[i][j].color);
            rectangle(MatrixBox2[i][j].pozX, MatrixBox2[i][j].pozY, MatrixBox2[i][j].pozX + MatrixBox2[i][j].width, MatrixBox2[i][j].pozY + MatrixBox2[i][j].height);
            setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
            settextjustify(CENTER_TEXT, CENTER_TEXT);
            outtextxy(MatrixBox2[i][j].pozX + MatrixBox2[i][j].width/2 + 1, MatrixBox2[i][j].pozY + MatrixBox2[i][j].height/2 + 5, MatrixBox2[i][j].text);
        }
    }
}

void showAllMbox2Inv(int LinMatrixBox, int ColMatrixBox)
{
    for (int i=1; i<=4; i++)
        MatrixBox2[i][1].pozX = 1000;
    for (int i=1; i<=4; i++)
        for (int j=1; j<=4; j++)
        {
            if(j!=1)
                MatrixBox2[i][j].pozX = MatrixBox2[i][j-1].pozX + MatrixBox2[i][j-1].width + 10;
        }

    bar(MatrixBox2[LinMatrixBox][LinMatrixBox].pozX, MatrixBox2[LinMatrixBox][LinMatrixBox].pozY - 4, MatrixBox2[4][4].pozX + MatrixBox2[4][4].width + 4, MatrixBox2[4][4].pozY + MatrixBox2[4][4].height + 4);

    for (int i = LinMatrixBox; i<=4; i++)
    {
        for (int j = 1; j<=4; j++)
        {
            if (showMatrixResultMult)
            {
                if (i<=countCol && j<=countCol2)
                    MatrixBox2[i][j].color = LIGHTRED, setcolor(MatrixBox2[i][j].color);
                else
                    MatrixBox2[i][j].color = settings.darkTheme?settings.textColor.dark:settings.textColor.light;
            }
            else
            {
                if (i<=countLin && j<=countCol)
                    MatrixBox2[i][j].color = LIGHTRED, setcolor(MatrixBox2[i][j].color);
                else
                    MatrixBox2[i][j].color = settings.darkTheme?settings.textColor.dark:settings.textColor.light;
            }

            setcolor(MatrixBox2[i][j].color);
            rectangle(MatrixBox2[i][j].pozX, MatrixBox2[i][j].pozY, MatrixBox2[i][j].pozX + MatrixBox2[i][j].width, MatrixBox2[i][j].pozY + MatrixBox2[i][j].height);
            setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
            settextjustify(CENTER_TEXT, CENTER_TEXT);
            outtextxy(MatrixBox2[i][j].pozX + MatrixBox2[i][j].width/2 + 1, MatrixBox2[i][j].pozY + MatrixBox2[i][j].height/2 + 5, MatrixBox2[i][j].text);
        }
    }
}

void showAllMboxResult(int LinMatrixBox, int ColMatrixBox)
{
    bar(MatrixBoxResult[LinMatrixBox][LinMatrixBox].pozX, MatrixBoxResult[LinMatrixBox][LinMatrixBox].pozY - 4, MatrixBoxResult[4][4].pozX + MatrixBoxResult[4][4].width + 4, MatrixBoxResult[4][4].pozY + MatrixBoxResult[4][4].height + 4);

    for (int i = LinMatrixBox; i<=4; i++)
    {
        for (int j = 1; j<=4; j++)
        {
            if (showMatrixResultMult)
            {
                if (i<=countLin && j<=countCol2)
                    MatrixBoxResult[i][j].color = LIGHTRED, setcolor(MatrixBoxResult[i][j].color);
                else
                    MatrixBoxResult[i][j].color = 1;
            }
            else
            {
                if (i<=countLin && j<=countCol)
                    MatrixBoxResult[i][j].color = LIGHTRED, setcolor(MatrixBoxResult[i][j].color);
                else
                    MatrixBoxResult[i][j].color = 1;
            }

            rectangle(MatrixBoxResult[i][j].pozX, MatrixBoxResult[i][j].pozY, MatrixBoxResult[i][j].pozX + MatrixBoxResult[i][j].width, MatrixBoxResult[i][j].pozY + MatrixBoxResult[i][j].height);
            setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
            settextjustify(CENTER_TEXT, CENTER_TEXT);
            outtextxy(MatrixBoxResult[i][j].pozX + MatrixBoxResult[i][j].width/2 + 1, MatrixBoxResult[i][j].pozY + MatrixBoxResult[i][j].height/2 + 5, MatrixBoxResult[i][j].text);
        }
    }
}

void createMatrixBox(char element[], int LinMatrixBox, int ColMatrixBox) //cream casuta primului element din vector
{
    settextstyle(settings.textFont, HORIZ_DIR, 3);
    settextjustify(CENTER_TEXT, CENTER_TEXT);

    strcpy(MatrixBox[LinMatrixBox][ColMatrixBox].text, element);

    showAllMbox(LinMatrixBox, ColMatrixBox);
}

void createMatrixBox2(char element[], int LinMatrixBox, int ColMatrixBox) //cream casuta primului element din vector
{
    settextstyle(settings.textFont, HORIZ_DIR, 3);
    settextjustify(CENTER_TEXT, CENTER_TEXT);

    strcpy(MatrixBox2[LinMatrixBox][ColMatrixBox].text, element);

    showAllMbox2(LinMatrixBox, ColMatrixBox);
}

void createMatrixBox2Inv(char element[], int LinMatrixBox, int ColMatrixBox) //cream casuta primului element din vector
{
    settextstyle(settings.textFont, HORIZ_DIR, 3);
    settextjustify(CENTER_TEXT, CENTER_TEXT);

    strcpy(MatrixBox2[LinMatrixBox][ColMatrixBox].text, element);

    showAllMbox2Inv(LinMatrixBox, ColMatrixBox);
}

void createMatrixBoxResult(char element[], int LinMatrixBox, int ColMatrixBox) //cream casuta primului element din vector
{
    settextstyle(settings.textFont, HORIZ_DIR, 3);
    settextjustify(CENTER_TEXT, CENTER_TEXT);

    strcpy(MatrixBoxResult[LinMatrixBox][ColMatrixBox].text, element);

    showAllMboxResult(LinMatrixBox, ColMatrixBox);
}

void requestNextVectorElement(char element[], int PosVectorBox)
{
    settextstyle(settings.textFont, HORIZ_DIR, 3);
    createVectorBox("_", PosVectorBox);
    mouseTrackerVector();
}

void requestVectorElement(int PosVectorBox, int mid) //se utilizeaza atunci cand se introduce un element in vector
{
    bool negative = false;
    VectorBox[PosVectorBox].show = true;
    VectorBox[PosVectorBox + 1].show = true;

    char element[100];

    int poz = 0;
    if(!requestChV)
        char c = getch();
    if(requestChV)
    {
        char tasta = getch();

        while (tasta != 13) //codul ASCII pt tasta enter este 13
        {
            if (tasta == 45 && poz == 0)
            {
                negative = true;
                element[poz] = tasta;
                poz++;
                element[poz] = '\0';
                VectorBox[PosVectorBox].width = textwidth(element) + 20;
                bar(VectorBox[PosVectorBox].pozX, VectorBox[PosVectorBox].pozY - 2, VectorBox[PosVectorBox].pozX + screenWidth, VectorBox[PosVectorBox].pozY + VectorBox[PosVectorBox].height + 2);
                createVectorBox(element, PosVectorBox);
            }

            if (tasta!=8 && isdigit(tasta) && !(isdigit(tasta) && tasta != '0' && VectorBox[PosVectorBox].text[0]=='0') && !(tasta=='0' && poz == 1 && VectorBox[PosVectorBox].text[0]=='-') &&!mid && VectorBox[PosVectorBox-1].pozX + VectorBox[PosVectorBox-1].width < screenWidth - 70) //codul ASCII pt tasta backspace este 8
            {
                element[poz] = tasta;
                poz++;
                element[poz] = '\0';
                VectorBox[PosVectorBox].width = textwidth(element) + 20;
                bar(VectorBox[PosVectorBox].pozX, VectorBox[PosVectorBox].pozY - 2, VectorBox[PosVectorBox].pozX + screenWidth, VectorBox[PosVectorBox].pozY + VectorBox[PosVectorBox].height + 2);
                if (VectorBox[PosVectorBox].pozX + VectorBox[PosVectorBox].width > screenWidth - 70)
                {
                    poz = strlen(VectorBox[PosVectorBox].text);
                    poz--;
                    if (poz != 0)
                        VectorBox[PosVectorBox].text[poz] = '\0';
                    else
                    {
                        VectorBox[PosVectorBox].text[0] = '_';
                        VectorBox[PosVectorBox].text[1] = '\0';
                    }
                }//strcpy(element, "_"), VectorBox[PosVectorBox].value = 0, createVectorBox[element, 1, 1];
                createVectorBox(element, PosVectorBox);
            }

            /*if (tasta == 8 && !mid)
            {
                poz--;
                element[poz] = '\0';
                VectorBox[PosVectorBox].width = textwidth(element) + 20;
                bar(VectorBox[PosVectorBox].pozX, VectorBox[PosVectorBox].pozY - 2, VectorBox[PosVectorBox].pozX + screenWidth, VectorBox[PosVectorBox].pozY + VectorBox[PosVectorBox].height + 2);
                createVectorBox(element, PosVectorBox);
            }*/

            if (tasta!=8 && mid && isdigit(tasta) && VectorBox[PosVectorBox-1].pozX + VectorBox[PosVectorBox-1].width < screenWidth - 70)
            {
                poz = strlen(VectorBox[PosVectorBox].text);
                if (VectorBox[PosVectorBox].text[poz-1] == '_')
                    poz--;
                VectorBox[PosVectorBox].text[poz] = tasta;
                poz++;
                VectorBox[PosVectorBox].text[poz] = '\0';
                VectorBox[PosVectorBox].width = textwidth(VectorBox[PosVectorBox].text) + 20;
                bar(VectorBox[PosVectorBox].pozX, VectorBox[PosVectorBox].pozY - 2, VectorBox[PosVectorBox].pozX + screenWidth, VectorBox[PosVectorBox].pozY + VectorBox[PosVectorBox].height + 2);
                if (VectorBox[PosVectorBox].pozX + VectorBox[PosVectorBox].width > screenWidth - 70)
                {
                    poz = strlen(VectorBox[PosVectorBox].text);
                    poz--;
                    if (poz != 0)
                        VectorBox[PosVectorBox].text[poz] = '\0';
                    else
                    {
                        VectorBox[PosVectorBox].text[0] = '_';
                        VectorBox[PosVectorBox].text[1] = '\0';
                    }
                }//strcpy(VectorBox[PosVectorBox].text, "_"), VectorBox[PosVectorBox].value = 0, createVectorBox[VectorBox[1].text, 1, 1];
                createVectorBox(VectorBox[PosVectorBox].text, PosVectorBox);
            }

            if (tasta==8)
            {
                poz = strlen(VectorBox[PosVectorBox].text);
                poz--;
                if (poz != 0)
                    VectorBox[PosVectorBox].text[poz] = '\0';
                else
                {
                    VectorBox[PosVectorBox].text[0] = '_';
                    VectorBox[PosVectorBox].text[1] = '\0';
                }
                VectorBox[PosVectorBox].width = textwidth(VectorBox[PosVectorBox].text) + 20;
                bar(VectorBox[PosVectorBox].pozX, VectorBox[PosVectorBox].pozY - 2, VectorBox[PosVectorBox].pozX + screenWidth, VectorBox[PosVectorBox].pozY + VectorBox[PosVectorBox].height + 2);
                createVectorBox(VectorBox[PosVectorBox].text, PosVectorBox);
            }

            tasta = getch();
        }
    }
    if (mid)
        strcpy(element, VectorBox[PosVectorBox].text);

    bar(VectorBox[1].pozX, VectorBox[1].pozY - 2, screenWidth, VectorBox[1].pozY + VectorBox[1].height + 2);
    if(!mid && VectorBox[PosVectorBox].text[0] == '_')
        VectorBox[PosVectorBox+1].show = false;
    createVectorBox(VectorBox[1].text, 1);

    VectorBox[PosVectorBox].value = 0;

    if (!negative)
        for (int i = 0; i<strlen(element); i++)
            VectorBox[PosVectorBox].value = VectorBox[PosVectorBox].value*10 + (int)element[i] - 48;
    else
    {
        for (int i = 1; i<strlen(element); i++)
            VectorBox[PosVectorBox].value = VectorBox[PosVectorBox].value*10 + (int)element[i] - 48;
        VectorBox[PosVectorBox].value = -VectorBox[PosVectorBox].value;
    }

    if (!mid && VectorBox[PosVectorBox].text[0] != '_' && VectorBox[PosVectorBox+1].text[0] == '_')
        requestNextVectorElement(element, PosVectorBox+1);//bar(VectorBox[PosVectorBox].pozX, VectorBox[PosVectorBox].pozY, VectorBox[PosVectorBox].pozX + screenWidth, VectorBox[PosVectorBox].pozY + VectorBox[PosVectorBox].height);
    else
        mouseTrackerVector();
}

void clearMatrix(int x) //matricea revine la forma initiala
{
    for (int i = 1; i<=4; i++)
        for (int j=1; j<=4; j++)
        {
            if (x==1)
                strcpy(MatrixBox[i][j].text, "_"), MatrixBox[i][j].value = 0, MatrixBox[i][j].color = settings.darkTheme?settings.textColor.dark:settings.textColor.light;
            if (x==2)
                strcpy(MatrixBox2[i][j].text, "_"), MatrixBox2[i][j].value = 0, MatrixBox2[i][j].color = settings.darkTheme?settings.textColor.dark:settings.textColor.light;
            if (x==3)
                strcpy(MatrixBoxResult[i][j].text, "_"), MatrixBoxResult[i][j].value = 0, MatrixBoxResult[i][j].color = settings.darkTheme?settings.textColor.dark:settings.textColor.light;
        }
}

void requestMatrixElement(int i, int j)
{
    bool negative = false;
    char element[100];
    int poz = 0;
    if(!requestChM)
        char c = getch();
    if(requestChM)
    {
        char tasta = getch();

        while (tasta != 13) //codul ASCII pt tasta enter este 13
        {
            if (tasta == 45 && poz == 0) //codul ASCII pt tasta - este 45
            {
                negative = true;
                poz = strlen(MatrixBox[i][j].text);
                if (MatrixBox[i][j].text[poz-1] == '_')
                    poz--;
                MatrixBox[i][j].text[poz] = tasta;
                poz++;
                MatrixBox[i][j].text[poz] = '\0';
                bar(MatrixBox[i][j].pozX, MatrixBox[i][j].pozY - 2, MatrixBox[4][4].pozX + MatrixBox[4][4].width + 4, MatrixBox[i][j].pozY + MatrixBox[i][j].height + 2);
                createMatrixBox(MatrixBox[i][j].text, i, j);
            }

            if (tasta!=8 && isdigit(tasta) && !(isdigit(tasta) && tasta != '0' && MatrixBox[i][j].text[0]=='0') && !(tasta=='0' && poz == 1 && MatrixBox[i][j].text[0]=='-')) //codul ASCII pt tasta backspace este 8
            {
                poz = strlen(MatrixBox[i][j].text);
                if (MatrixBox[i][j].text[poz-1] == '_')
                    poz--;
                MatrixBox[i][j].text[poz] = tasta;
                poz++;
                MatrixBox[i][j].text[poz] = '\0';
                bar(MatrixBox[i][j].pozX, MatrixBox[i][j].pozY - 2, MatrixBox[4][4].pozX + MatrixBox[4][4].width + 4, MatrixBox[i][j].pozY + MatrixBox[i][j].height + 2);
                if (textwidth(MatrixBox[i][j].text)<MatrixBox[i][j].width)
                    createMatrixBox(MatrixBox[i][j].text, i, j);
                else
                {
                    poz = strlen(MatrixBox[i][j].text);
                    poz--;
                    if (poz != 0)
                        MatrixBox[i][j].text[poz] = '\0';
                    else
                    {
                        MatrixBox[i][j].text[0] = '_';
                        MatrixBox[i][j].text[1] = '\0';
                    }
                    bar(MatrixBox[i][j].pozX, MatrixBox[i][j].pozY - 2, MatrixBox[4][4].pozX + MatrixBox[4][4].width + 4, MatrixBox[i][j].pozY + MatrixBox[i][j].height + 2);
                    createMatrixBox(MatrixBox[i][j].text, i, j);
                }
            }

            if (tasta==8)
            {
                poz = strlen(MatrixBox[i][j].text);
                poz--;
                if (poz != 0)
                    MatrixBox[i][j].text[poz] = '\0';
                else
                {
                    MatrixBox[i][j].text[0] = '_';
                    MatrixBox[i][j].text[1] = '\0';
                }
                bar(MatrixBox[i][j].pozX, MatrixBox[i][j].pozY - 2, MatrixBox[4][4].pozX + MatrixBox[4][4].width + 4, MatrixBox[i][j].pozY + MatrixBox[i][j].height + 2);
                createMatrixBox(MatrixBox[i][j].text, i, j);
            }

            tasta = getch();
        }
    }
    MatrixBox[i][j].value = 0;

    if (!negative)
        for (int k = 0; k<strlen(MatrixBox[i][j].text); k++)
            MatrixBox[i][j].value = MatrixBox[i][j].value*10 + (int)MatrixBox[i][j].text[k] - 48;
    else
    {
        for (int k = 1; k<strlen(MatrixBox[i][j].text); k++)
            MatrixBox[i][j].value = MatrixBox[i][j].value*10 + (int)MatrixBox[i][j].text[k] - 48;
        MatrixBox[i][j].value = -MatrixBox[i][j].value;
    }

    FilledMatrix = true;
    for (int i = 1; i<=countLin; i++)
        for (int j =1; j<=countCol; j++)
        {
            if (strcmp(MatrixBox[i][j].text, "_")==0)
                FilledMatrix = false;
        }

    if (FilledMatrix)
    {
        Check_ShowResultSum();
        Check_ShowResultMult();
    }

    mouseTrackerMatrix();
}

void Check_ShowResultMult() //se utilizeaza in cadrul inmultirii a doua matrici
{
    bool eroare = false;
    if (showMatrixResultMult)
    {
        showMatrixResultSum = false;
        eroare = false;
        countCol2 = 0;
        int nrm = -1, prev_nrm = -1;
        for (int i = 1; i<=countCol; i++)
        {
            if (strcmp(MatrixBox2[i][1].text, "_")!=0)
                if (nrm == -1)
                    nrm = 1;
                else nrm++;
        }
        if(nrm == countCol)
            countCol2 = 1;
        else
            delay(200), setcolor(1), eroare = 1, countCol2 = 0;

        prev_nrm = -1, nrm = -1;
        if (countCol2)
            for (int j=2; j<=4; j++)
            {
                prev_nrm = nrm;
                nrm = 0;
                for (int i = 1; i<=countCol; i++)
                {
                    if (strcmp(MatrixBox2[i][j].text, "_")!=0)
                        nrm++;
                }
                if (prev_nrm == -1)
                {
                    if(nrm == countCol)
                        countCol2++;
                }
                else
                {
                    if (prev_nrm == nrm && nrm == countCol)
                        countCol2++;
                }
                if (nrm && nrm!=countCol)
                    eroare = 1;
            }
    }

    if (showMatrixResultMult && !eroare && FilledMatrix)
    {
        createMatrixBox2(MatrixBox2[1][1].text, 1, 1);
        clearMatrix(3);
        outtextxy(MatrixBox2[2][4].pozX + MatrixBox2[2][4].width + 20, MatrixBox2[2][4].pozY + MatrixBox2[2][4].height/2, "=");
        int sum = 0;
        for(int i=1; i<=countLin; i++)
            for (int j=1; j<=countCol2; j++)
            {
                sum = 0;
                for (int k=1; k<=countCol; k++)
                {
                    animateMbox(i, k, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                    animateMbox2(k, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                    sum += MatrixBox[i][k].value * MatrixBox2[k][j].value;
                    NumbertoChar(sum, functionCall_v);
                    if(settings.playFXSounds)
                        PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);
                    bar(MatrixBoxResult[i][j].pozX + 1, MatrixBoxResult[i][j].pozY + 1, MatrixBoxResult[i][j].pozX + MatrixBoxResult[i][j].width - 1, MatrixBoxResult[i][j].pozY + MatrixBoxResult[i][j].height - 1);
                    createMatrixBoxResult(functionCall_v, i, j);
                    delay(100);
                    exitanimateMbox(i, k);
                    exitanimateMbox2(k, j);
                }
                delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
            }
    }
}

void Check_ShowResultSum() //se utilizeaza in cadrul adunarii a doua matrici
{
    bool completedSum = true;
    for (int i = 1; i<=countLin; i++)
        for (int j=1; j<=countCol; j++)
        {
            if (strcmp(MatrixBox2[i][j].text, "_")==0)
                completedSum = false;
        }
    if (completedSum)
    {
        outtextxy(MatrixBox2[2][4].pozX + MatrixBox2[2][4].width + 20, MatrixBox2[2][4].pozY + MatrixBox2[2][4].height/2, "=");
        createMatrixBoxResult(MatrixBoxResult[1][1].text, 1, 1);
        exitanimateButton(ButtonSum_M);
    }

    if (showMatrixResultSum)
    {
        showMatrixResultMult = false;
    }
    if (showMatrixResultSum && completedSum && FilledMatrix)
    {
        createMatrixBox2(MatrixBox2[1][1].text, 1, 1);
        completedSum = false;
        for(int i=1; i<=countLin; i++)
            for (int j=1; j<=countCol; j++)
            {
                animateMbox(i, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                animateMbox2(i, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                int sum = MatrixBox[i][j].value + MatrixBox2[i][j].value;
                NumbertoChar(sum, functionCall_v);
                if(settings.playFXSounds)
                    PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);
                bar(MatrixBoxResult[i][j].pozX + 1, MatrixBoxResult[i][j].pozY + 1, MatrixBoxResult[i][j].pozX + MatrixBoxResult[i][j].width - 1, MatrixBoxResult[i][j].pozY + MatrixBoxResult[i][j].height - 1);
                createMatrixBoxResult(functionCall_v, i, j);
                delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
                exitanimateMbox(i, j);
                delay(100);
                exitanimateMbox2(i, j);
            }
    }
}

void ShowResultTrans() //transpua matricei
{
    int i, j;
    for (i=1; i<=countLin; i++)
        for (j=1; j<=countCol; j++)
        {
            MatrixBox2[j][i].color = LIGHTRED;
        }

    for (i=1; i<=countLin; i++)
        for(j=1; j<=countCol; j++)
        {
            animateMbox(i, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);

            MatrixBox2[j][i].value = MatrixBox[i][j].value, strcpy(MatrixBox2[j][i].text, MatrixBox[i][j].text);
            createMatrixBox2(MatrixBox2[1][1].text, 1, 1);
            animateMbox2(j, i, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
            delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);

            exitanimateMbox(i, j);
            delay(100);
            exitanimateMbox2(j, i);

        }

    for (i=1; i<=countLin; i++)
        for(j=1; j<=countCol; j++)
            MatrixBox[i][j].color = settings.darkTheme?settings.textColor.dark:settings.textColor.light;

    swap(countLin, countCol);
    clearMatrix(1);
    for (i=1; i<=countLin; i++)
        for(j=1; j<=countCol; j++)
        {
            animateMbox2(i, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
            MatrixBox[i][j].color = LIGHTRED, setcolor(MatrixBox[i][j].color);

            MatrixBox[i][j].value = MatrixBox2[i][j].value, strcpy(MatrixBox[i][j].text, MatrixBox2[i][j].text);
            createMatrixBox(MatrixBox[1][1].text, 1, 1);
            animateMbox(i, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
            delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);

            exitanimateMbox2(i, j);
            delay(100);
            exitanimateMbox(i, j);
        }

    clearMatrix(2);
    if(settings.playFXSounds)
        PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);

    NumbertoChar(countLin, functionCall_v);
    bar(ButtonNLin_M.pozX + ButtonNLin_M.width + 4, ButtonNLin_M.pozY - 1, screenWidth, ButtonNLin_M.pozY + ButtonNLin_M.height);
    outtextxy(ButtonNLin_M.pozX + ButtonNLin_M.width + 20, ButtonNLin_M.pozY + 20, functionCall_v);

    NumbertoChar(countCol, functionCall_v);
    bar(ButtonNCol_M.pozX + ButtonNCol_M.width + 4, ButtonNCol_M.pozY - 1, screenWidth, ButtonNCol_M.pozY + ButtonNCol_M.height);
    outtextxy(ButtonNCol_M.pozX + ButtonNCol_M.width + 20, ButtonNCol_M.pozY + 20, functionCall_v);

    bar(MatrixBox[1][4].pozX + MatrixBox[1][4].width + 1, MatrixBox[1][1].pozY - 3, screenWidth, MatrixBox[4][4].pozY + MatrixBox[4][4].height + 3);
    mouseTrackerMatrix();
}

void ShowResultDet() //determinantul matricei
{
    int offset = MatrixBox[2][4].pozX + MatrixBox[2][4].width + 45;

    if (countLin != countCol || countLin==0 ||countCol ==0)
    {
        settextjustify(LEFT_TEXT, CENTER_TEXT);
        setcolor(LIGHTRED);
        outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, "Matricea nu este patratica!");
        if(settings.playFXSounds) PlaySound(TEXT("error.wav"), NULL, SND_FILENAME|SND_ASYNC);
        settextjustify(CENTER_TEXT, CENTER_TEXT);
        delay(500);
        bar(MatrixBox[1][4].pozX + MatrixBox[1][4].width + 1, MatrixBox[1][1].pozY - 3, screenWidth, MatrixBox[4][4].pozY + MatrixBox[4][4].height + 3);
        setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
        return;
    }

    if (countLin == 1)
    {
        animateMbox(1, 1, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
        setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
        settextstyle(settings.textFont, HORIZ_DIR, 3);
        settextjustify(CENTER_TEXT, CENTER_TEXT);
        if(settings.playFXSounds)
            PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);
        outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, MatrixBox[1][1].text);
        delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
        exitanimateMbox(1, 1);
    }

    if (countLin == 2)
    {
        int s;
        outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, "("), offset = offset + textwidth("(") + 10;
        for (int k = 1; k<=2; k++)
        {
            animateMbox(k, k, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
            setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
            settextstyle(settings.textFont, HORIZ_DIR, 3);
            settextjustify(CENTER_TEXT, CENTER_TEXT);
            outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, MatrixBox[k][k].text);
            delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
            offset = offset + textwidth(MatrixBox[k][k].text) + 10;
            if (k==2)
                outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, ")"), offset = offset + textwidth(")") + 10;
            else
                outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, "*"), offset = offset + textwidth("*") + 10;
            exitanimateMbox(k, k);
        }

        setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
        outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, "-"), offset = offset + textwidth("-") + 10;
        outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, "("), offset = offset + textwidth("(") + 10;

        for (int k = 1; k<=2; k++)
        {
            animateMbox(k, countLin+1-k, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
            setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
            settextstyle(settings.textFont, HORIZ_DIR, 3);
            settextjustify(CENTER_TEXT, CENTER_TEXT);
            outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, MatrixBox[k][countLin+1-k].text);
            delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
            offset = offset + textwidth(MatrixBox[k][countLin+1-k].text) + 10;
            if (k==2)
                outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, ")"), offset = offset + textwidth(")") + 10;
            else
                outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, "*"), offset = offset + textwidth("*") + 10;
            exitanimateMbox(k, countLin+1-k);
        }

        setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
        outtextxy(offset,MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, "="), offset = offset + textwidth("=") + 10;

        s = MatrixBox[1][1].value * MatrixBox[2][2].value - MatrixBox[1][2].value * MatrixBox[2][1].value;
        NumbertoChar(s, functionCall_v);
        setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
        settextjustify(CENTER_TEXT, CENTER_TEXT);
        if(settings.playFXSounds)
            PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);
        outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, functionCall_v);
        delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
    }

    if (countLin == 3)
    {
        offset = MatrixBox[2][4].pozX + MatrixBox[2][4].width + 45;
        int offsetR = offset;
        settextjustify(LEFT_TEXT, CENTER_TEXT);
        outtextxy(offsetR, MatrixBox[4][4].pozY, "Determinantul");
        offsetR += textwidth("Determinantul") + 10;
        int i, j, s = 0, p;
        for (int k = 1; k<=countCol; k++)
        {
            outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, "("), offset = offset + textwidth("(") + 10;
            i = 1, j = k;
            p = 1;
            while(i<=countLin)
            {
                p *= MatrixBox[i][j].value;

                animateMbox(i, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
                settextstyle(settings.textFont, HORIZ_DIR, 3);
                settextjustify(CENTER_TEXT, CENTER_TEXT);
                outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, MatrixBox[i][j].text);
                delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
                offset = offset + textwidth(MatrixBox[i][j].text) + 10;
                if (i == countLin)
                    outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, ")"), offset = offset + textwidth(")") + 10;
                else
                    outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, "*"), offset = offset + textwidth("*") + 10;
                i++;
                j++;
                if (j==4)
                    j=1;
            }

            i = 1, j = k;
            while(i<=countLin)
            {
                exitanimateMbox(i, j);
                i++;
                j++;
                if (j==4)
                    j=1;
            }
            s += p;

            setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
            if (k==countCol)
                offset = MatrixBox[2][4].pozX + MatrixBox[2][4].width + 45, outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2 + 20, "-"), offset = offset + textwidth("-") + 10;
            else outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, "+"), offset = offset + textwidth("+") + 10;
        }

        for (int k = 1; k<=countCol; k++)
        {
            outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2 + 20, "("), offset = offset + textwidth("(") + 10;
            i = 3, j = k;
            p = 1;
            while(i>=1)
            {
                p *= MatrixBox[i][j].value;
                animateMbox(i, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
                settextstyle(settings.textFont, HORIZ_DIR, 3);
                settextjustify(CENTER_TEXT, CENTER_TEXT);
                outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2 + 20, MatrixBox[i][j].text);
                delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
                offset = offset + textwidth(MatrixBox[i][j].text) + 10;
                if (i == 1)
                    outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2 + 20, ")"), offset = offset + textwidth(")") + 10;
                else
                    outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2 + 20, "*"), offset = offset + textwidth("*") + 10;

                i--;
                j++;
                if (j==4)
                    j=1;
            }

            i = 3, j = k;
            while(i>=1)
            {
                exitanimateMbox(i, j);
                i--;
                j++;
                if (j==4)
                    j=1;
            }

            s -= p;

            setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
            if (k!=countCol)
                outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2 + 20, "-"), offset = offset + textwidth("-") + 10;
        }

        NumbertoChar(s, functionCall_v);
        if(settings.playFXSounds)
            PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);
        outtextxy(offsetR, MatrixBox[4][4].pozY, "="), offsetR += textwidth("=") + 10;
        setcolor(LIGHTGREEN);
        settextstyle(EUROPEAN_FONT, HORIZ_DIR, 4);
        settextjustify(LEFT_TEXT, CENTER_TEXT);
        outtextxy(offsetR, MatrixBox[4][4].pozY, functionCall_v);
        setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
        settextstyle(EUROPEAN_FONT, HORIZ_DIR, 3);
        settextjustify(CENTER_TEXT, CENTER_TEXT);

        offset = MatrixBox[2][4].pozX + MatrixBox[2][4].width + 45;
        bar(offset-40, MatrixBox[1][1].pozY - 3, screenWidth, MatrixBox[2][2].pozY + MatrixBox[2][2].height + 20);
    }

    if (countLin == 4)
    {
        offset = MatrixBox[2][4].pozX + MatrixBox[2][4].width + 45;
        int offsetR = offset;
        settextjustify(LEFT_TEXT, CENTER_TEXT);
        outtextxy(offsetR, MatrixBox[4][4].pozY, "Determinantul = ");
        offsetR += textwidth("Determinantul = ") + 10;

        settextstyle(EUROPEAN_FONT, HORIZ_DIR, 1);
        int i, j, s = 0, p, J, k, sgn, counter = 0, bannedI, bannedJ, S = 0, counterR = 0;
        int offsety;

        for (J=1; J<=4; J++)
        {
            offsety = 20;
            s = 0;
            createMatrixBox(MatrixBox[1][1].text, 1, 1);
            settextstyle(EUROPEAN_FONT, HORIZ_DIR, 1);
            offset = MatrixBox[2][4].pozX + MatrixBox[2][4].width + 45;
            bar(offset-3, MatrixBox[1][1].pozY - 3, screenWidth, MatrixBox[2][2].pozY + MatrixBox[2][2].height + 3);
            animateMbox(1, J, LIGHTGREEN);

            sgn = (1+J)%2;
            bannedI = 1, bannedJ = J;

            line(MatrixBox[1][1].pozX, MatrixBox[1][1].pozY + MatrixBox[1][1].height/2, MatrixBox[1][4].pozX + MatrixBox[1][4].width, MatrixBox[1][4].pozY + MatrixBox[1][4].height/2);
            line(MatrixBox[1][J].pozX + MatrixBox[1][J].width/2, MatrixBox[1][J].pozY, MatrixBox[4][J].pozX + MatrixBox[4][J].width/2, MatrixBox[4][J].pozY + MatrixBox[4][J].height);

            for (k = 1; k<=countCol; k++)
            {
                i = 1, j = 1;
                while (i==bannedI)
                    i++;
                if (k==bannedJ)
                    k++;
                j = k;
                if (k<=countCol)
                {
                    outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, "("), offset = offset + textwidth("(") + 10;

                    p = 1, counter = 0;

                    while(counter<3)
                    {
                        counter++;
                        p *= MatrixBox[i][j].value;

                        animateMbox(i, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                        setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
                        settextjustify(LEFT_TEXT, CENTER_TEXT);
                        outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, MatrixBox[i][j].text);
                        delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
                        offset = offset + textwidth(MatrixBox[i][j].text) + 10;
                        if (counter == 3)
                            outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, ")"), offset = offset + textwidth(")") + 10;
                        else
                            outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, "*"), offset = offset + textwidth("*") + 10;
                        i++;
                        if (i==bannedI)
                            i++;

                        j++;
                        if (j==bannedJ && bannedJ != 4)
                            j++;
                        else if (j==bannedJ && bannedJ == 4)
                            j = 1;
                        else if (j==5)
                        {
                            j=1;
                            while(j==bannedJ)
                                j++;
                        }
                    }

                    i = 1, j = 1;
                    while (i==bannedI)
                        i++;
                    if (k==bannedJ)
                        k++;
                    j = k;

                    counter = 0;
                    while(counter<3)
                    {
                        exitanimateMbox(i, j);
                        counter++;
                        i++;
                        if (i==bannedI)
                            i++;

                        j++;
                        if (j==bannedJ && bannedJ != 4)
                            j++;
                        else if (j==bannedJ && bannedJ == 4)
                            j = 1;
                        else if (j==5)
                        {
                            j=1;
                            while(j==bannedJ)
                                j++;
                        }
                    }
                    s += p;
                }
                setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
                if (k == countCol || k==5)
                    offset = MatrixBox[2][4].pozX + MatrixBox[2][4].width + 45, outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2 + offsety, "-"), offset = offset + textwidth("-") + 10;
                else outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, "+"), offset = offset + textwidth("+") + 10;

            }

            for (k = 1; k<=countCol; k++)////////////////////////////////////////////////////////////////////////////////////////////////////////
            {
                i = countLin, j = 1;
                while (i==bannedI)
                    i--;
                if (k==bannedJ)
                    k++;
                j = k;
                if (k<=countCol)
                {
                    outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2 + offsety, "("), offset = offset + textwidth("(") + 10;

                    p = 1, counter = 0;

                    while(counter<3)
                    {
                        counter++;
                        p *= MatrixBox[i][j].value;

                        animateMbox(i, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                        setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
                        settextjustify(LEFT_TEXT, CENTER_TEXT);
                        outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2 + offsety, MatrixBox[i][j].text);
                        delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
                        offset = offset + textwidth(MatrixBox[i][j].text) + 10;
                        if (counter == 3)
                            outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2 + offsety, ")"), offset = offset + textwidth(")") + 10;
                        else
                            outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2 + offsety, "*"), offset = offset + textwidth("*") + 10;
                        i--;
                        if (i==bannedI)
                            i--;

                        j++;
                        if (j==bannedJ && bannedJ != 4)
                            j++;
                        else if (j==bannedJ && bannedJ == 4)
                            j = 1;
                        else if (j==5)
                        {
                            j=1;
                            while(j==bannedJ)
                                j++;
                        }
                    }

                    i = countLin, j = 1;
                    while (i==bannedI)
                        i--;
                    if (k==bannedJ)
                        k++;
                    j = k;

                    counter = 0;
                    while(counter<3)
                    {
                        exitanimateMbox(i, j);
                        counter++;
                        i--;
                        if (i==bannedI)
                            i--;

                        j++;
                        if (j==bannedJ && bannedJ != 4)
                            j++;
                        else if (j==bannedJ && bannedJ == 4)
                            j = 1;
                        else if (j==5)
                        {
                            j=1;
                            while(j==bannedJ)
                                j++;
                        }
                    }
                    s -= p;
                }
                setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);

                if (k>3)
                    settextjustify(RIGHT_TEXT, CENTER_TEXT), outtextxy(offset + 5, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2 + offsety, "="), offset = offset + textwidth("=") + 10;
                else if (k<=3)outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2 + offsety, "-"), offset = offset + textwidth("-") + 10;

            }

            NumbertoChar(s, functionCall_v);
            setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
            settextjustify(CENTER_TEXT, CENTER_TEXT);
            outtextxy(offset + 5, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2 + offsety, functionCall_v);

            exitanimateMbox(1, J);

            counterR++;
            offsety = 0;
            setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
            settextjustify(LEFT_TEXT, CENTER_TEXT);

            if (counterR ==5)
                offsetR = MatrixBox[2][4].pozX + MatrixBox[2][4].width + 45;
            if (counterR >= 5)
                offsety = 20;

            if (sgn == 0)
            {
                outtextxy(offsetR, MatrixBox[4][4].pozY + offsety, "+"), offsetR += textwidth("+") + 10;
                outtextxy(offsetR, MatrixBox[4][4].pozY + offsety, MatrixBox[1][J].text), offsetR += textwidth(MatrixBox[1][J].text) + 10;
                outtextxy(offsetR, MatrixBox[4][4].pozY + offsety, "*"), offsetR += textwidth("*") + 10;
                outtextxy(offsetR, MatrixBox[4][4].pozY + offsety, functionCall_v), offsetR += textwidth(functionCall_v) + 10;
                S += MatrixBox[1][J].value * s;
            }
            if (sgn == 1)
            {
                outtextxy(offsetR, MatrixBox[4][4].pozY + offsety, "-"), offsetR += textwidth("-") + 10;
                outtextxy(offsetR, MatrixBox[4][4].pozY + offsety, MatrixBox[1][J].text), offsetR += textwidth(MatrixBox[1][J].text) + 10;
                outtextxy(offsetR, MatrixBox[4][4].pozY + offsety, "*"), offsetR += textwidth("*") + 10;
                outtextxy(offsetR, MatrixBox[4][4].pozY + offsety, functionCall_v), offsetR += textwidth(functionCall_v) + 10;
                S -= MatrixBox[1][J].value * s;
            }

            delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
        }

        offsetR = MatrixBox[2][4].pozX + MatrixBox[2][4].width + 45;
        NumbertoChar(S, functionCall_v);
        if(settings.playFXSounds)
            PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);
        outtextxy(offsetR, MatrixBox[4][4].pozY + 20, "="), offsetR += textwidth("=") + 10;
        setcolor(LIGHTGREEN);
        settextstyle(EUROPEAN_FONT, HORIZ_DIR, 4);
        settextjustify(LEFT_TEXT, CENTER_TEXT);
        outtextxy(offsetR, MatrixBox[4][4].pozY + 30, functionCall_v);
        setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
        settextstyle(EUROPEAN_FONT, HORIZ_DIR, 3);
        settextjustify(CENTER_TEXT, CENTER_TEXT);

        offset = MatrixBox[2][4].pozX + MatrixBox[2][4].width + 45;
        bar(offset-40, MatrixBox[1][1].pozY - 3, screenWidth, MatrixBox[2][2].pozY + MatrixBox[2][2].height + 3);
    }
    createMatrixBox(MatrixBox[1][1].text, 1, 1);

    mouseTrackerMatrix();
}

void ShowResultDetInv() //determinantul matricei in cadrul functiei de inversare
{
    int offset = MatrixBox2[2][4].pozX + MatrixBox2[2][4].width + 45;

    int offsetR = offset;
    settextjustify(LEFT_TEXT, CENTER_TEXT);

    settextstyle(EUROPEAN_FONT, HORIZ_DIR, 1);
    int i, j, s = 0, p, J, I, k, sgn, counter = 0, bannedI, bannedJ, S = 0, counterR = 0;
    int offsety;

    if (countLin == 1)
    {
        animateMbox2(1, 1, LIGHTGREEN), delay(100), exitanimateMbox2(1, 1), createMatrixBox2(MatrixBox[1][1].text, 1, 1), animateMbox2(1, 1, LIGHTGREEN), delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow), exitanimateMbox2(1, 1);
        return;
    }

    if (countLin == 4)
        for (I=1; I<=countLin; I++)
            for (J=1; J<=countCol; J++)
            {
                offsety = 20;
                s = 0;
                createMatrixBox(MatrixBox[1][1].text, 1, 1);
                settextstyle(EUROPEAN_FONT, HORIZ_DIR, 1);
                offset = MatrixBox2[2][4].pozX + MatrixBox2[2][4].width + 45;
                bar(offset-3, MatrixBox2[1][1].pozY - 3, screenWidth, MatrixBox2[2][2].pozY + MatrixBox2[2][2].height + 3);
                animateMbox(I, J, LIGHTGREEN);

                sgn = (I+J)%2;
                bannedI = I, bannedJ = J;

                line(MatrixBox[I][1].pozX, MatrixBox[I][1].pozY + MatrixBox[I][1].height/2, MatrixBox[I][4].pozX + MatrixBox[I][4].width, MatrixBox[I][4].pozY + MatrixBox[I][4].height/2);
                line(MatrixBox[1][J].pozX + MatrixBox[1][J].width/2, MatrixBox[1][J].pozY, MatrixBox[4][J].pozX + MatrixBox[4][J].width/2, MatrixBox[4][J].pozY + MatrixBox[4][J].height);

                settextjustify(LEFT_TEXT, CENTER_TEXT);
                outtextxy(offset, MatrixBox2[2][4].pozY + MatrixBox2[2][4].height/2, "(-1)^"), offset += textwidth("(-1)^") + 3;
                NumbertoChar(I+J, functionCall_v);
                outtextxy(offset, MatrixBox2[2][4].pozY + MatrixBox2[2][4].height/2, functionCall_v), offset += textwidth(functionCall_v) + 7;

                for (k = 1; k<=countCol; k++)
                {
                    i = 1, j = 1;
                    while (i==bannedI)
                        i++;
                    if (k==bannedJ)
                        k++;
                    j = k;
                    if (k<=countCol)
                    {
                        p = 1, counter = 0;

                        while(counter<3)
                        {
                            counter++;
                            p *= MatrixBox[i][j].value;

                            animateMbox(i, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                            delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);

                            i++;
                            if (i==bannedI)
                                i++;

                            j++;
                            if (j==bannedJ && bannedJ != 4)
                                j++;
                            else if (j==bannedJ && bannedJ == 4)
                                j = 1;
                            else if (j==5)
                            {
                                j=1;
                                while(j==bannedJ)
                                    j++;
                            }
                        }

                        i = 1, j = 1;
                        while (i==bannedI)
                            i++;
                        if (k==bannedJ)
                            k++;
                        j = k;

                        counter = 0;
                        while(counter<3)
                        {
                            exitanimateMbox(i, j);
                            counter++;
                            i++;
                            if (i==bannedI)
                                i++;

                            j++;
                            if (j==bannedJ && bannedJ != 4)
                                j++;
                            else if (j==bannedJ && bannedJ == 4)
                                j = 1;
                            else if (j==5)
                            {
                                j=1;
                                while(j==bannedJ)
                                    j++;
                            }
                        }
                        s += p;
                    }
                    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
                }

                for (k = 1; k<=countCol; k++)
                {
                    i = countLin, j = 1;
                    while (i==bannedI)
                        i--;
                    if (k==bannedJ)
                        k++;
                    j = k;
                    if (k<=countCol)
                    {
                        p = 1, counter = 0;

                        while(counter<3)
                        {
                            counter++;
                            p *= MatrixBox[i][j].value;

                            animateMbox(i, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                            delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);

                            i--;
                            if (i==bannedI)
                                i--;

                            j++;
                            if (j==bannedJ && bannedJ != 4)
                                j++;
                            else if (j==bannedJ && bannedJ == 4)
                                j = 1;
                            else if (j==5)
                            {
                                j=1;
                                while(j==bannedJ)
                                    j++;
                            }
                        }

                        i = countLin, j = 1;
                        while (i==bannedI)
                            i--;
                        if (k==bannedJ)
                            k++;
                        j = k;

                        counter = 0;
                        while(counter<3)
                        {
                            exitanimateMbox(i, j);
                            counter++;
                            i--;
                            if (i==bannedI)
                                i--;

                            j++;
                            if (j==bannedJ && bannedJ != 4)
                                j++;
                            else if (j==bannedJ && bannedJ == 4)
                                j = 1;
                            else if (j==5)
                            {
                                j=1;
                                while(j==bannedJ)
                                    j++;
                            }
                        }
                        s -= p;
                    }
                }

                exitanimateMbox(I, J);

                counterR++;

                S = pow(-1, I+J) * s;

                delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);

                NumbertoChar(S, functionCall_v);
                createMatrixBox2(functionCall_v, I, J);
                settextjustify(LEFT_TEXT, CENTER_TEXT);
                settextstyle(EUROPEAN_FONT, HORIZ_DIR, 1);
                outtextxy(offset, MatrixBox2[2][4].pozY + MatrixBox2[2][4].height/2, "*"), offset += textwidth("*") + 7;
                outtextxy(offset, MatrixBox2[2][4].pozY + MatrixBox2[2][4].height/2, functionCall_v);
                delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
            }

    if (countLin == 3)
        for (I=1; I<=countLin; I++)
            for (J=1; J<=countCol; J++)
            {
                offsety = 20;
                s = 0;
                createMatrixBox(MatrixBox[1][1].text, 1, 1);
                settextstyle(EUROPEAN_FONT, HORIZ_DIR, 1);
                offset = MatrixBox2[2][4].pozX + MatrixBox2[2][4].width + 45;
                bar(offset-7, MatrixBox2[1][1].pozY - 3, screenWidth, MatrixBox2[2][2].pozY + MatrixBox2[2][2].height + 3);
                animateMbox(I, J, LIGHTGREEN);

                sgn = (I+J)%2;
                bannedI = I, bannedJ = J;

                line(MatrixBox[I][1].pozX, MatrixBox[I][1].pozY + MatrixBox[I][1].height/2, MatrixBox[I][3].pozX + MatrixBox[I][3].width, MatrixBox[I][3].pozY + MatrixBox[I][3].height/2);
                line(MatrixBox[1][J].pozX + MatrixBox[1][J].width/2, MatrixBox[1][J].pozY, MatrixBox[3][J].pozX + MatrixBox[3][J].width/2, MatrixBox[3][J].pozY + MatrixBox[3][J].height);

                settextjustify(LEFT_TEXT, CENTER_TEXT);
                outtextxy(offset, MatrixBox2[2][4].pozY + MatrixBox2[2][4].height/2, "(-1)^"), offset += textwidth("(-1)^") + 3;
                NumbertoChar(I+J, functionCall_v);
                outtextxy(offset, MatrixBox2[2][4].pozY + MatrixBox2[2][4].height/2, functionCall_v), offset += textwidth(functionCall_v) + 7;


                i = 1, j = 1;
                while (i==bannedI)
                    i++;
                while (j==bannedJ)
                    j++;

                p = 1;
                p *= MatrixBox[i][j].value;

                animateMbox(i, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);

                i++;
                if (i==bannedI)
                    i++;

                j++;
                if (j==bannedJ)
                    j++;

                p *= MatrixBox[i][j].value;

                animateMbox(i, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);


                i = 1, j = 1;
                while (i==bannedI)
                    i++;
                while (j==bannedJ)
                    j++;

                exitanimateMbox(i, j);

                i++;
                if (i==bannedI)
                    i++;

                j++;
                if (j==bannedJ)
                    j++;

                exitanimateMbox(i, j);

                s += p;

                setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);


                i = countLin, j = 1;
                while (i==bannedI)
                    i--;
                while (j==bannedJ)
                    j++;

                p = 1;
                p *= MatrixBox[i][j].value;

                animateMbox(i, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);

                i--;
                if (i==bannedI)
                    i--;

                j++;
                if (j==bannedJ)
                    j++;

                p *= MatrixBox[i][j].value;

                animateMbox(i, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);


                i = countLin, j = 1;
                while (i==bannedI)
                    i--;
                while (j==bannedJ)
                    j++;

                exitanimateMbox(i, j);

                i--;
                if (i==bannedI)
                    i--;

                j++;
                if (j==bannedJ)
                    j++;

                exitanimateMbox(i, j);

                s -= p;

                setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);

                exitanimateMbox(I, J);

                S = pow(-1, I+J) * s;

                NumbertoChar(S, functionCall_v);
                createMatrixBox2(functionCall_v, I, J);
                settextjustify(LEFT_TEXT, CENTER_TEXT);
                settextstyle(EUROPEAN_FONT, HORIZ_DIR, 1);
                outtextxy(offset, MatrixBox2[2][4].pozY + MatrixBox2[2][4].height/2, "*"), offset += textwidth("*") + 7;
                outtextxy(offset, MatrixBox2[2][4].pozY + MatrixBox2[2][4].height/2, functionCall_v);
                delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
            }

    if (countLin == 2)
        for (I=1; I<=countLin; I++)
            for (J=1; J<=countCol; J++)
            {
                offsety = 20;
                s = 0;
                createMatrixBox(MatrixBox[1][1].text, 1, 1);
                settextstyle(EUROPEAN_FONT, HORIZ_DIR, 1);
                offset = MatrixBox2[2][4].pozX + MatrixBox2[2][4].width + 45;
                bar(offset-7, MatrixBox2[1][1].pozY - 3, screenWidth, MatrixBox2[2][2].pozY + MatrixBox2[2][2].height + 3);
                animateMbox(I, J, LIGHTGREEN);

                sgn = (I+J)%2;
                bannedI = I, bannedJ = J;

                line(MatrixBox[I][1].pozX, MatrixBox[I][1].pozY + MatrixBox[I][1].height/2, MatrixBox[I][2].pozX + MatrixBox[I][2].width, MatrixBox[I][2].pozY + MatrixBox[I][2].height/2);
                line(MatrixBox[1][J].pozX + MatrixBox[1][J].width/2, MatrixBox[1][J].pozY, MatrixBox[2][J].pozX + MatrixBox[2][J].width/2, MatrixBox[2][J].pozY + MatrixBox[2][J].height);

                settextjustify(LEFT_TEXT, CENTER_TEXT);
                outtextxy(offset, MatrixBox2[2][4].pozY + MatrixBox2[2][4].height/2, "(-1)^"), offset += textwidth("(-1)^") + 3;
                NumbertoChar(I+J, functionCall_v);
                outtextxy(offset, MatrixBox2[2][4].pozY + MatrixBox2[2][4].height/2, functionCall_v), offset += textwidth(functionCall_v) + 7;


                i = 1, j = 1;
                while (i==bannedI)
                    i++;
                while (j==bannedJ)
                    j++;

                animateMbox(i, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);

                i = 1, j = 1;
                while (i==bannedI)
                    i++;
                while (j==bannedJ)
                    j++;
                exitanimateMbox(i, j);

                s = MatrixBox[i][j].value;

                setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);

                i = countLin, j = 1;
                while (i==bannedI)
                    i--;
                while (j==bannedJ)
                    j++;

                animateMbox(i, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);

                i = countLin, j = 1;
                while (i==bannedI)
                    i--;
                while (j==bannedJ)
                    j++;
                exitanimateMbox(I, J);

                s = MatrixBox[i][j].value;

                setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);

                S = pow(-1, I+J) * s;

                NumbertoChar(S, functionCall_v);
                createMatrixBox2(functionCall_v, I, J);
                settextjustify(LEFT_TEXT, CENTER_TEXT);
                settextstyle(EUROPEAN_FONT, HORIZ_DIR, 1);
                outtextxy(offset, MatrixBox2[2][4].pozY + MatrixBox2[2][4].height/2, "*"), offset += textwidth("*") + 7;
                outtextxy(offset, MatrixBox2[2][4].pozY + MatrixBox2[2][4].height/2, functionCall_v);
                delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
            }

    offset = MatrixBox2[2][4].pozX + MatrixBox2[2][4].width + 45;
    bar(offset-10, MatrixBox2[1][1].pozY - 3, screenWidth, MatrixBox2[2][2].pozY + MatrixBox2[2][2].height + 3);
    createMatrixBox(MatrixBox[1][1].text, 1, 1);
    createMatrixBox2(MatrixBox2[1][1].text, 1, 1);

    return;
}

void ShowResultInv() //inversa matricei
{
    int det, offset = MatrixBox[2][4].pozX + MatrixBox[2][4].width + 45;

    int AuxMatrix[5][5];
    for (int i=1; i<=countLin; i++)
        for (int j=1; j<=countCol; j++)
            AuxMatrix[i][j] = MatrixBox[i][j].value;

    if (countLin != countCol || countLin==0 ||countCol ==0)
    {
        settextjustify(LEFT_TEXT, CENTER_TEXT);
        setcolor(LIGHTRED);
        outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, "Matricea nu este patratica!");
        if(settings.playFXSounds) PlaySound(TEXT("error.wav"), NULL, SND_FILENAME|SND_ASYNC);
        settextjustify(CENTER_TEXT, CENTER_TEXT);
        delay(500);
        bar(MatrixBox[1][4].pozX + MatrixBox[1][4].width + 1, MatrixBox[1][1].pozY - 3, screenWidth, MatrixBox[4][4].pozY + MatrixBox[4][4].height + 3);
        setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
        return;
    }

    if (countLin == 1)
    {
        det = MatrixBox[1][1].value;
    }

    if (countLin == 2)
    {
        det = MatrixBox[1][1].value * MatrixBox[2][2].value - MatrixBox[1][2].value * MatrixBox[2][1].value;
    }

    if (countLin == 3)
    {
        int i, j, s = 0, p;
        for (int k = 1; k<=countCol; k++)
        {
            i = 1, j = k;
            p = 1;
            while(i<=countLin)
            {
                p *= MatrixBox[i][j].value;

                i++;
                j++;
                if (j==4)
                    j=1;
            }

            s += p;
        }

        for (int k = 1; k<=countCol; k++)
        {
            i = 3, j = k;
            p = 1;
            while(i>=1)
            {
                p *= MatrixBox[i][j].value;

                i--;
                j++;
                if (j==4)
                    j=1;
            }

            s -= p;
        }

        det = s;
    }

    if (countLin == 4)
    {
        int i, j, s = 0, p, J, k, sgn, counter = 0, bannedI, bannedJ, S = 0, counterR = 0;
        int offsety;

        for (J=1; J<=4; J++)
        {
            s = 0;
            sgn = (1+J)%2;
            bannedI = 1, bannedJ = J;

            for (k = 1; k<=countCol; k++)
            {
                i = 1, j = 1;
                while (i==bannedI)
                    i++;
                if (k==bannedJ)
                    k++;
                j = k;
                if (k<=countCol)
                {
                    p = 1, counter = 0;

                    while(counter<3)
                    {
                        counter++;
                        p *= MatrixBox[i][j].value;

                        i++;
                        if (i==bannedI)
                            i++;

                        j++;
                        if (j==bannedJ && bannedJ != 4)
                            j++;
                        else if (j==bannedJ && bannedJ == 4)
                            j = 1;
                        else if (j==5)
                        {
                            j=1;
                            while(j==bannedJ)
                                j++;
                        }
                    }

                    s += p;
                }
            }

            for (k = 1; k<=countCol; k++)
            {
                i = countLin, j = 1;
                while (i==bannedI)
                    i--;
                if (k==bannedJ)
                    k++;
                j = k;
                if (k<=countCol)
                {
                    p = 1, counter = 0;

                    while(counter<3)
                    {
                        counter++;
                        p *= MatrixBox[i][j].value;

                        i--;
                        if (i==bannedI)
                            i--;

                        j++;
                        if (j==bannedJ && bannedJ != 4)
                            j++;
                        else if (j==bannedJ && bannedJ == 4)
                            j = 1;
                        else if (j==5)
                        {
                            j=1;
                            while(j==bannedJ)
                                j++;
                        }
                    }

                    s -= p;
                }

            }

            counterR++;

            if (sgn == 0)
            {
                S += MatrixBox[1][J].value * s;
            }
            if (sgn == 1)
            {
                S -= MatrixBox[1][J].value * s;
            }
        }

        det = S;
    }

    NumbertoChar(det, functionCall_v);
    offset = MatrixBox[2][4].pozX + MatrixBox[2][4].width + 45;
    settextjustify(LEFT_TEXT, CENTER_TEXT);
    outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, "Determinantul = ");
    offset += textwidth("Determinantul = ") + 10;
    outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, functionCall_v);
    offset += textwidth(functionCall_v) + 30;
    delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);

    if (det == 0)
    {
        offset = MatrixBox[2][4].pozX + MatrixBox[2][4].width + 45;
        setcolor(LIGHTRED);
        outtextxy(offset, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2 + 20, "Eroare! Determinantul este nul!");
        if(settings.playFXSounds) PlaySound(TEXT("error.wav"), NULL, SND_FILENAME|SND_ASYNC);
        delay(1000);
        bar(offset-15, MatrixBox[1][1].pozY - 3, screenWidth, MatrixBox[2][4].pozY + MatrixBox[2][4].height + 3);
        return;
    }

    createMatrixBox(MatrixBox[1][1].text, 1, 1);
    settextjustify(CENTER_TEXT, TOP_TEXT);
    outtextxy((MatrixBox2[1][1].pozX + MatrixBox2[1][4].pozX + MatrixBox2[1][4].width)/2, MatrixBox2[4][4].pozY + MatrixBox2[4][4].height + 10, "Matricea transpusa");
    if(settings.playFXSounds)
        PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);

    int i, j;
    for (i=1; i<=countLin; i++)
        for (j=1; j<=countCol; j++)
        {
            MatrixBox2[j][i].color = LIGHTRED;
        }

    for (i=1; i<=countLin; i++)
        for(j=1; j<=countCol; j++)
        {
            animateMbox(i, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);

            MatrixBox2[j][i].value = MatrixBox[i][j].value, strcpy(MatrixBox2[j][i].text, MatrixBox[i][j].text);
            createMatrixBox2(MatrixBox2[1][1].text, 1, 1);
            animateMbox2(j, i, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
            delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);

            exitanimateMbox(i, j);
            delay(100);
            exitanimateMbox2(j, i);
        }

    clearMatrix(1);
    for (i=1; i<=countLin; i++)
        for(j=1; j<=countCol; j++)
        {
            animateMbox2(i, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
            MatrixBox[i][j].color = LIGHTRED, setcolor(MatrixBox[i][j].color);

            MatrixBox[i][j].value = MatrixBox2[i][j].value, strcpy(MatrixBox[i][j].text, MatrixBox2[i][j].text);
            createMatrixBox(MatrixBox[1][1].text, 1, 1);
            animateMbox(i, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);

            exitanimateMbox2(i, j);
            exitanimateMbox(i, j);
        }

    clearMatrix(2);
    setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);

    settextjustify(CENTER_TEXT, TOP_TEXT);
    bar(MatrixBox2[4][1].pozX, MatrixBox2[4][4].pozY + MatrixBox2[4][4].height + 5, screenWidth, ButtonNCol_M.pozY);
    outtextxy((MatrixBox2[1][1].pozX + MatrixBox2[1][4].pozX + MatrixBox2[1][4].width)/2, MatrixBox2[4][4].pozY + MatrixBox2[4][4].height + 10, "Matricea adjuncta");
    if(settings.playFXSounds)
        PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);

    ShowResultDetInv();

    settextjustify(CENTER_TEXT, TOP_TEXT);
    bar(MatrixBox[1][4].pozX + MatrixBox[1][4].width + 1, MatrixBox[1][1].pozY - 3, screenWidth, MatrixBox[4][4].pozY + MatrixBox[4][4].height + 3);

    bar(MatrixBox2[4][1].pozX, MatrixBox2[4][4].pozY + MatrixBox2[4][4].height + 5, screenWidth, ButtonNCol_M.pozY);
    if(settings.playFXSounds)
        PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);
    outtextxy((MatrixBox2[1][1].pozX + MatrixBox2[1][4].pozX + MatrixBox2[1][4].width)/2, MatrixBox2[4][4].pozY + MatrixBox2[4][4].height + 10, "Matricea Inversa");
    createMatrixBox2Inv(MatrixBox2[1][1].text, 1, 1);

    NumbertoChar(det, functionCall_v);
    line(MatrixBox2[1][1].pozX - 80, MatrixBox2[2][2].pozY + MatrixBox2[2][2].height, MatrixBox2[1][1].pozX - 10, MatrixBox2[2][2].pozY + MatrixBox2[2][2].height);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    outtextxy(MatrixBox2[1][1].pozX - 45, MatrixBox2[2][2].pozY + MatrixBox2[2][2].height - 10, "1");
    outtextxy(MatrixBox2[1][1].pozX - 45, MatrixBox2[2][2].pozY + MatrixBox2[2][2].height + 25, functionCall_v);

    for (int i=1; i<=countLin; i++)
        for (int j=1; j<=countCol; j++)
            MatrixBox[i][j].value = AuxMatrix[i][j], NumbertoChar(MatrixBox[i][j].value, functionCall_v), strcpy(MatrixBox[i][j].text, functionCall_v);
    createMatrixBox(MatrixBox[1][1].text, 1, 1);


    for (int i=1; i<=4; i++)
        MatrixBox2[i][1].pozX = 800;
    for (int i=1; i<=4; i++)
        for (int j=1; j<=4; j++)
        {
            if(j!=1)
                MatrixBox2[i][j].pozX = MatrixBox2[i][j-1].pozX + MatrixBox2[i][j-1].width + 10;
        }
}

void ShowResultPow(int n) //ridicarea la putere a matricei
{
    int i, j, k, pow;

    if (countLin != countCol || countLin==0 ||countCol ==0)
    {
        settextjustify(LEFT_TEXT, CENTER_TEXT);
        setcolor(LIGHTRED);
        outtextxy(MatrixBox[2][4].pozX + MatrixBox[2][4].width + 45, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, "Matricea nu este patratica!");
        if(settings.playFXSounds) PlaySound(TEXT("error.wav"), NULL, SND_FILENAME|SND_ASYNC);
        settextjustify(CENTER_TEXT, CENTER_TEXT);
        delay(500);
        bar(MatrixBox[1][4].pozX + MatrixBox[1][4].width + 1, MatrixBox[1][1].pozY - 3, screenWidth, MatrixBox[4][4].pozY + MatrixBox[4][4].height + 3);
        setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
        return;
    }

    settextjustify(CENTER_TEXT, CENTER_TEXT);
    outtextxy(MatrixBox[2][4].pozX + MatrixBox[2][4].width + 20, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, "*");
    int AuxMatrix[5][5];
    for (int i=1; i<=countLin; i++)
        for (int j=1; j<=countCol; j++)
            AuxMatrix[i][j] = MatrixBox[i][j].value, strcpy(MatrixBox2[i][j].text, MatrixBox[i][j].text), MatrixBox2[i][j].value = MatrixBox[i][j].value;


    if (n == 0)
    {
        for (i=1; i<=countLin; i++)
            for (j=1; j<=countCol; j++)
            {
                strcpy(MatrixBoxResult[i][j].text, "0"), MatrixBoxResult[i][j].value = 0;
                if (i == j)
                    strcpy(MatrixBoxResult[i][j].text, "1"), MatrixBoxResult[i][j].value = 1;
            }

        settextjustify(LEFT_TEXT, CENTER_TEXT);
        settextstyle(EUROPEAN_FONT, HORIZ_DIR, 7);
        bar(MatrixBox[1][4].pozX + MatrixBox[1][4].width + 3, MatrixBox[1][4].pozY -3, screenWidth, MatrixBox[4][4].pozY + MatrixBox[4][4].height + 3);
        outtextxy(MatrixBox[2][4].pozX + MatrixBox[2][4].width + 20, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, "^");
        NumbertoChar(n, functionCall_v);
        outtextxy(MatrixBox[2][4].pozX + MatrixBox[2][4].width + 70, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, functionCall_v);
        outtextxy(MatrixBox[2][4].pozX + MatrixBox[2][4].width + 120, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, "=");
        settextstyle(EUROPEAN_FONT, HORIZ_DIR, 3);
        createMatrixBoxResult(MatrixBoxResult[1][1].text, 1, 1);
        settextjustify(CENTER_TEXT, TOP_TEXT);
        outtextxy((MatrixBoxResult[1][1].pozX + MatrixBoxResult[1][4].pozX + MatrixBoxResult[1][4].width)/2, MatrixBoxResult[4][4].pozY + MatrixBoxResult[4][4].height + 10, "Matricea identica");
        settextjustify(CENTER_TEXT, CENTER_TEXT);
        if(settings.playFXSounds)
            PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);
    }
    else
        for (pow = 1; pow<n || pow==1; pow++)
        {
            if(settings.playFXSounds)
                PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);
            if (n!=1)
            {
                createMatrixBox2(MatrixBox2[1][1].text, 1, 1);
                clearMatrix(3);
                outtextxy(MatrixBox2[2][4].pozX + MatrixBox2[2][4].width + 20, MatrixBox2[2][4].pozY + MatrixBox2[2][4].height/2, "=");
                int sum = 0;
                for(i=1; i<=countLin; i++)
                    for (j=1; j<=countCol; j++)
                    {
                        sum = 0;
                        for (k=1; k<=countCol; k++)
                        {
                            animateMbox(i, k, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                            animateMbox2(k, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                            sum += MatrixBox[i][k].value * MatrixBox2[k][j].value;
                            NumbertoChar(sum, functionCall_v);
                            MatrixBoxResult[i][j].value = sum;
                            //bar(MatrixBoxResult[i][j].pozX + 1, MatrixBoxResult[i][j].pozY + 1, MatrixBoxResult[i][j].pozX + MatrixBoxResult[i][j].width - 1, MatrixBoxResult[i][j].pozY + MatrixBoxResult[i][j].height - 1);
                            createMatrixBoxResult(functionCall_v, i, j);
                            delay(100);
                            exitanimateMbox(i, k);
                            exitanimateMbox2(k, j);
                        }
                        delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
                    }

                for (i=1; i<=countLin; i++)
                    for (j=1; j<=countCol; j++)
                        strcpy(MatrixBox[i][j].text, MatrixBoxResult[i][j].text), MatrixBox[i][j].value = MatrixBoxResult[i][j].value;
            }

            if (pow==n-1 || n==1)
            {
                if (n==1)
                {
                    for (i=1; i<=countLin; i++)
                        for (j=1; j<=countCol; j++)
                            strcpy(MatrixBoxResult[i][j].text, MatrixBox[i][j].text), MatrixBoxResult[i][j].value = MatrixBox[i][j].value;

                }

                for (i=1; i<=countLin; i++)
                    for (j=1; j<=countCol; j++)
                        MatrixBox[i][j].value = AuxMatrix[i][j], NumbertoChar(MatrixBox[i][j].value, functionCall_v), strcpy(MatrixBox[i][j].text, functionCall_v);
                createMatrixBox(MatrixBox[1][1].text, 1, 1);

                settextjustify(LEFT_TEXT, CENTER_TEXT);
                settextstyle(EUROPEAN_FONT, HORIZ_DIR, 7);
                bar(MatrixBox[1][4].pozX + MatrixBox[1][4].width + 3, MatrixBox[1][4].pozY -3, screenWidth, MatrixBox[4][4].pozY + MatrixBox[4][4].height + 3);
                outtextxy(MatrixBox[2][4].pozX + MatrixBox[2][4].width + 20, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, "^");
                NumbertoChar(n, functionCall_v);
                outtextxy(MatrixBox[2][4].pozX + MatrixBox[2][4].width + 60, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, functionCall_v);
                outtextxy(MatrixBox[2][4].pozX + MatrixBox[2][4].width + 100, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, "=");
                settextstyle(EUROPEAN_FONT, HORIZ_DIR, 3);
                settextjustify(CENTER_TEXT, CENTER_TEXT);
                createMatrixBoxResult(MatrixBoxResult[1][1].text, 1, 1);
            }
            else
            {
                bar(MatrixBox[1][4].pozX + MatrixBox[1][4].width + 3, MatrixBox[1][4].pozY -3, screenWidth, MatrixBox[4][4].pozY + MatrixBox[4][4].height + 3);
                createMatrixBox(MatrixBox[1][1].text, 1, 1);
                outtextxy(MatrixBox[2][4].pozX + MatrixBox[2][4].width + 20, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, "*");
            }

        }

}
void requestMatrixElement2(int i, int j)
{
    bool negative = false;
    char element[100];
    int poz = 0;
    if(!requestChM)
        char c = getch();
    if(requestChM)
    {
        char tasta = getch();

        while (tasta != 13) //codul ASCII pt tasta enter este 13
        {
            if (tasta == 45 && poz == 0)
            {
                negative = true;
                poz = strlen(MatrixBox2[i][j].text);
                if (MatrixBox2[i][j].text[poz-1] == '_' || MatrixBox2[i][j].text[poz-1] == '_')
                    poz--;
                MatrixBox2[i][j].text[poz] = tasta;
                poz++;
                MatrixBox2[i][j].text[poz] = '\0';
                bar(MatrixBox2[i][j].pozX, MatrixBox2[i][j].pozY - 2, MatrixBox2[4][4].pozX + MatrixBox2[4][4].width + 4, MatrixBox2[i][j].pozY + MatrixBox2[i][j].height + 2);
                createMatrixBox2(MatrixBox2[i][j].text, i, j);
            }

            if (tasta!=8 && isdigit(tasta) && !(isdigit(tasta) && tasta != '0' && MatrixBox2[i][j].text[0]=='0') && !(tasta=='0' && poz == 1 && MatrixBox2[i][j].text[0]=='-'))
            {
                poz = strlen(MatrixBox2[i][j].text);
                if (MatrixBox2[i][j].text[poz-1] == '_' || MatrixBox2[i][j].text[poz-1] == '_')
                    poz--;
                MatrixBox2[i][j].text[poz] = tasta;
                poz++;
                MatrixBox2[i][j].text[poz] = '\0';
                bar(MatrixBox2[i][j].pozX, MatrixBox2[i][j].pozY - 2, MatrixBox2[4][4].pozX + MatrixBox2[4][4].width + 4, MatrixBox2[i][j].pozY + MatrixBox2[i][j].height + 2);
                if (textwidth(MatrixBox2[i][j].text)<MatrixBox2[i][j].width)
                    createMatrixBox2(MatrixBox2[i][j].text, i, j);
                else
                {
                    poz = strlen(MatrixBox2[i][j].text);
                    poz--;
                    if (poz != 0)
                        MatrixBox2[i][j].text[poz] = '\0';
                    else
                    {
                        MatrixBox2[i][j].text[0] = '_';
                        MatrixBox2[i][j].text[1] = '\0';
                    }
                    bar(MatrixBox2[i][j].pozX, MatrixBox2[i][j].pozY - 2, MatrixBox2[4][4].pozX + MatrixBox2[4][4].width + 4, MatrixBox2[i][j].pozY + MatrixBox2[i][j].height + 2);
                    createMatrixBox2(MatrixBox2[i][j].text, i, j);
                }
            }

            if (tasta==8)
            {
                poz = strlen(MatrixBox2[i][j].text);
                poz--;
                if (poz != 0)
                    MatrixBox2[i][j].text[poz] = '\0';
                else
                {
                    MatrixBox2[i][j].text[0] = '_';
                    MatrixBox2[i][j].text[1] = '\0';
                }
                bar(MatrixBox2[i][j].pozX, MatrixBox2[i][j].pozY - 2, MatrixBox2[4][4].pozX + MatrixBox2[4][4].width + 4, MatrixBox2[i][j].pozY + MatrixBox2[i][j].height + 2);
                createMatrixBox2(MatrixBox2[i][j].text, i, j);
            }

            tasta = getch();
            clearMatrix(3);
        }
    }

    MatrixBox2[i][j].value = 0;

    if (!negative)
        for (int k = 0; k<strlen(MatrixBox2[i][j].text); k++)
            MatrixBox2[i][j].value = MatrixBox2[i][j].value*10 + (int)MatrixBox2[i][j].text[k] - 48;
    else
    {
        for (int k = 1; k<strlen(MatrixBox2[i][j].text); k++)
            MatrixBox2[i][j].value = MatrixBox2[i][j].value*10 + (int)MatrixBox2[i][j].text[k] - 48;
        MatrixBox2[i][j].value = -MatrixBox2[i][j].value;
    }

    Check_ShowResultSum();
    Check_ShowResultMult();

    mouseTrackerMatrix();
}

int changeVectorScreen, changeMatrixScreen;
void mouseTrackerVector() //verificam unde s-a dat click in cadrul ecranului de vectori
{
    changeVectorScreen = 0;
    getmouseclick(WM_LBUTTONDOWN, mouse.x, mouse.y);
    for(int PosVectorBox = 1; PosVectorBox <= 1000; PosVectorBox++)
    {
        if (mouse.x > VectorBox[PosVectorBox].pozX && mouse.y > VectorBox[PosVectorBox].pozY && mouse.x < (VectorBox[PosVectorBox].pozX + VectorBox[PosVectorBox].width) && mouse.y < (VectorBox[PosVectorBox].pozY + VectorBox[PosVectorBox].height))
            if (strcmp(VectorBox[PosVectorBox].text, "_") == 0)
                animateVbox(PosVectorBox, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light), requestChV = true, countV++, requestVectorElement(PosVectorBox, 0), changeVectorScreen = 1;
            else
            {
                animateVbox(PosVectorBox, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light), requestChV = true, requestVectorElement(PosVectorBox, 1), changeVectorScreen = 1;
            }
        requestChV = false;
    }

    if (!changeVectorScreen)
    {
        if (verifyButton(ButtonMatrix))
        {
            MatrixScreen();
            startVector = false;
            startMatrix = true;
        }
        if (verifyButton(ButtonSettings))
        {

            SettingsScreen();
            startVector = false;
            startSettings = true;
        }

        if (verifyButton(ButtonInstructions))
        {
            InstructionsScreen();
            startVector = false;
            startInstructions = true;
        }

        if (verifyButton(ButtonExit))
        {
            startVector = false;
            Exit = true;
        }

        if (verifyButton(ButtonSum_V))
        {
            animateButton(ButtonSum_V);
            bar(ButtonSum_V.pozX + ButtonSum_V.width + 4, ButtonSum_V.pozY, ButtonSum_V.pozX + screenWidth, ButtonSum_V.pozY  + ButtonSum_V.height);
            functionCall_v[0] = '\0';
            int i, offset = ButtonSum_V.pozX + ButtonSum_V.width + 20;
            if (countV)
            {
                for (i = 1; i<=countV; i++)
                {
                    Sum_V(functionCall_v, i);
                    animateVbox(i, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light), exitanimateVbox(i);
                    settextjustify(LEFT_TEXT, CENTER_TEXT);
                    outtextxy(offset, ButtonSum_V.pozY + 20, VectorBox[i].text);
                    delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
                    offset = offset + textwidth(VectorBox[i].text) + 10;
                    if (i != countV)
                        outtextxy(offset, ButtonSum_V.pozY + 20, "+"), offset = offset + textwidth("+") + 10;
                    else
                    {
                        if(settings.playFXSounds)
                            PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);
                        outtextxy(offset, ButtonSum_V.pozY + 20, "="), offset = offset + textwidth("=") + 10;
                    }
                }

                Sum_V(functionCall_v, countV);
                settextjustify(LEFT_TEXT, CENTER_TEXT);
                outtextxy(offset, ButtonSum_V.pozY + 20, functionCall_v);
            }
            else
            {
                outtextxy(offset, ButtonSum_V.pozY + 20, "0"), offset = offset + textwidth("0") + 10;
            }
            delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);

            exitanimateButton(ButtonSum_V);
        }

        if (verifyButton(ButtonMult_V))
        {
            animateButton(ButtonMult_V);
            bar(ButtonMult_V.pozX + ButtonMult_V.width + 4, ButtonMult_V.pozY, ButtonMult_V.pozX + screenWidth, ButtonMult_V.pozY  + ButtonMult_V.height);
            functionCall_v[0] = '\0';
            int i, offset = ButtonMult_V.pozX + ButtonMult_V.width + 20;
            if (countV)
            {
                for (i = 1; i<=countV; i++)
                {
                    Sum_V(functionCall_v, i);
                    animateVbox(i, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light), exitanimateVbox(i);
                    settextjustify(LEFT_TEXT, CENTER_TEXT);
                    outtextxy(offset, ButtonMult_V.pozY + 20, VectorBox[i].text);
                    delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
                    offset = offset + textwidth(VectorBox[i].text) + 10;
                    if (i != countV)
                        outtextxy(offset, ButtonMult_V.pozY + 20, "*"), offset = offset + textwidth("+") + 10;
                    else
                    {
                        if(settings.playFXSounds)
                            PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);
                        outtextxy(offset, ButtonMult_V.pozY + 20, "="), offset = offset + textwidth("=") + 10;
                    }
                }

                Mult_V(functionCall_v, countV);
                settextjustify(LEFT_TEXT, CENTER_TEXT);
                outtextxy(offset, ButtonMult_V.pozY + 20, functionCall_v);
            }
            else
            {
                outtextxy(offset, ButtonMult_V.pozY + 20, "0"), offset = offset + textwidth("0") + 10;
            }
            delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);

            exitanimateButton(ButtonMult_V);
        }

        if (verifyButton(ButtonScalar_V))
        {
            animateButton(ButtonScalar_V);
            int scalar = RequestNumber_V(ButtonScalar_V);

            for (int i = 1; i<=countV; i++)
            {
                VectorBox[i].value *= scalar;
                NumbertoChar(VectorBox[i].value, functionCall_v);
                createVectorBox(functionCall_v, i);
                animateVbox(i, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                if(settings.playFXSounds)
                    PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);
                delay(100);
                exitanimateVbox(i);
                delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
            }
            exitanimateButton(ButtonScalar_V);
        }

        if (verifyButton(ButtonBubbleSort_V))
        {
            animateButton(ButtonBubbleSort_V);
            BubbleSort_V(VectorBox, countV);
            exitanimateButton(ButtonBubbleSort_V);
        }

        if (verifyButton(ButtonQuickSort_V))
        {
            animateButton(ButtonQuickSort_V);
            QuickSort_V(VectorBox, 1, countV);
            exitanimateButton(ButtonQuickSort_V);
        }

        if (verifyButton(ButtonShift_R))
        {
            animateButton(ButtonShift_R);
            if (countV)
                Shift_R(VectorBox, countV);
            exitanimateButton(ButtonShift_R);
        }

        if (verifyButton(ButtonShift_L))
        {
            animateButton(ButtonShift_L);
            if (countV)
                Shift_L(VectorBox, countV);
            exitanimateButton(ButtonShift_L);
        }

        if (verifyButton(ButtonElim_V))
        {
            animateButton(ButtonElim_V);
            int pos = RequestNumber_V(ButtonElim_V);
            if ((strcmp(VectorBox[1].text, "_") == 0 && pos == 1) || pos>countV)
            {
                setcolor(LIGHTRED), outtextxy(ButtonElim_V.pozX + ButtonElim_V.width + 60, ButtonElim_V.pozY + 20, "Pozitie gresita!"), setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
                if(settings.playFXSounds)
                    PlaySound(TEXT("error.wav"), NULL, SND_FILENAME|SND_ASYNC);
            }
            else
            {
                Elim_V(VectorBox, pos, countV);
                if(settings.playFXSounds)
                    PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);
            }

            exitanimateButton(ButtonElim_V);
        }

        if (verifyButton(ButtonSavedFile_V))
        {
            if(settings.playFXSounds)
                PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);
            animateButton(ButtonSavedFile_V);
            ifstream SavedFileV_in ("savedfileVector.txt");
            int valoare;
            SavedFileV_in>>countV;
            for (int i = 1; i <= countV+1; i++)
                VectorBox[i].width = textwidth("_") + 20, strcpy(VectorBox[i].text, "_");
            for (int i = 1; i <= countV+1; i++)
            {
                VectorBox[i].show = true;
            }

            for(int i = 1; i<=countV; i++)
            {
                SavedFileV_in>>valoare;
                VectorBox[i].value = valoare;
                NumbertoChar(valoare, functionCall_v);
                createVectorBox(functionCall_v, i);
                animateVbox(i, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                delay(100);
                exitanimateVbox(i);
                delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
            }
            exitanimateButton(ButtonSavedFile_V);
        }
    }
}

void mouseTrackerMatrix() //verificam unde s-a dat click in cadrul ecranului de matrici
{
    getmouseclick(WM_LBUTTONDOWN, mouse.x, mouse.y);

    changeMatrixScreen = 0;
    settextstyle(settings.textFont, HORIZ_DIR, 2);
    settextjustify(CENTER_TEXT, CENTER_TEXT);

    for (int i=1; i<=countLin; i++)
        for(int j=1; j<=countCol; j++)
        {
            if (mouse.x>MatrixBox[i][j].pozX && mouse.y>MatrixBox[i][j].pozY && mouse.x<MatrixBox[i][j].pozX + MatrixBox[i][j].width && mouse.y<MatrixBox[i][j].pozY + MatrixBox[i][j].height)
                if (strcmp(MatrixBox[i][j].text, "_") == 0)
                    animateMbox(i, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light), requestChM = true, requestMatrixElement(i, j), changeMatrixScreen = 1;
                else
                {
                    animateMbox(i, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light), requestChM = true, requestMatrixElement(i, j), changeMatrixScreen = 1;
                }
            requestChM = false;
        }

    if (showMatrix2)
    {
        if (showMatrixResultMult)
        {
            for (int i=1; i<=countCol; i++)
                for (int j=1; j<=4; j++)
                {
                    if (mouse.x>MatrixBox2[i][j].pozX && mouse.y>MatrixBox2[i][j].pozY && mouse.x<MatrixBox2[i][j].pozX + MatrixBox2[i][j].width && mouse.y<MatrixBox2[i][j].pozY + MatrixBox2[i][j].height)
                        if (strcmp(MatrixBox2[i][j].text, "_") == 0)
                            animateMbox2(i, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light), requestChM = true, requestMatrixElement2(i, j), changeMatrixScreen = 1;
                        else
                        {
                            animateMbox2(i, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light), requestChM = true, requestMatrixElement2(i, j), changeMatrixScreen = 1;
                        }
                    requestChM = false;
                }
        }
        else
            for (int i=1; i<=countLin; i++)
                for (int j=1; j<=countCol; j++)
                {
                    if (mouse.x>MatrixBox2[i][j].pozX && mouse.y>MatrixBox2[i][j].pozY && mouse.x<MatrixBox2[i][j].pozX + MatrixBox2[i][j].width && mouse.y<MatrixBox2[i][j].pozY + MatrixBox2[i][j].height)
                        if (strcmp(MatrixBox2[i][j].text, "_") == 0)
                            animateMbox2(i, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light), requestChM = true, requestMatrixElement2(i, j), changeMatrixScreen = 1;
                        else
                        {
                            animateMbox2(i, j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light), requestChM = true, requestMatrixElement2(i, j), changeMatrixScreen = 1;
                        }
                    requestChM = false;
                }
    }

    if (!changeMatrixScreen)
    {
        createMatrixBox(MatrixBox[1][1].text, 1, 1);
        //if (!showMatrixResultMult && !showMatrixResultSum)
            //bar(MatrixBox[4][4].pozX + MatrixBox[4][4].width + 3, MatrixBox[1][1].pozY - 3, screenWidth, MatrixBox[4][4].pozY + MatrixBox[4][4].height + 3);

        if (verifyButton(ButtonVector))
        {
            VectorScreen();
            startMatrix = false;
            startVector = true;
        }
        if (verifyButton(ButtonSettings))
        {
            SettingsScreen();
            startMatrix = false;
            startSettings = true;
        }

        if (verifyButton(ButtonInstructions))
        {
            InstructionsScreen();
            startMatrix = false;
            startInstructions = true;
        }
        if (verifyButton(ButtonExit))
        {
            startMatrix = false;
            Exit = true;
        }

        if (verifyButton(ButtonNLin_M))
        {
            exitanimateButton(ButtonSum_M);
            exitanimateButton(ButtonMult_M);
            exitanimateButton(ButtonSumElem_M);
            SumButtons = false;
            showMatrix2 = false;
            animateButton(ButtonNLin_M);
            int n = RequestNumber_V(ButtonNLin_M);
            if (n>4 || n<1)
            {
                setcolor(LIGHTRED), outtextxy(ButtonNLin_M.pozX + ButtonNLin_M.width + 60, ButtonNLin_M.pozY + 20, "Imposibil!"), setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
                if(settings.playFXSounds)
                    PlaySound(TEXT("error.wav"), NULL, SND_FILENAME|SND_ASYNC);
            }
            else
                countLin = n;
            exitanimateButton(ButtonNLin_M);
            clearMatrix(1);
            createMatrixBox(MatrixBox[1][1].text, 1, 1);
            bar(MatrixBox[1][4].pozX + MatrixBox[1][4].width + 3, MatrixBox[1][4].pozY -3, screenWidth, MatrixBox[4][4].pozY + MatrixBox[4][4].height + 3);
        }
        if (verifyButton(ButtonNCol_M))
        {
            exitanimateButton(ButtonSum_M);
            exitanimateButton(ButtonMult_M);
            exitanimateButton(ButtonSumElem_M);
            SumButtons = false;
            showMatrix2 = false;
            animateButton(ButtonNCol_M);
            int n = RequestNumber_V(ButtonNCol_M);
            if (n>4 || n<1)
            {
                setcolor(LIGHTRED), outtextxy(ButtonNCol_M.pozX + ButtonNCol_M.width + 60, ButtonNCol_M.pozY + 20, "Imposibil!"), setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
                if(settings.playFXSounds)
                    PlaySound(TEXT("error.wav"), NULL, SND_FILENAME|SND_ASYNC);
            }
            else
                countCol = n;
            exitanimateButton(ButtonNCol_M);
            clearMatrix(1);
            createMatrixBox(MatrixBox[1][1].text, 1, 1);
            bar(MatrixBox[1][4].pozX + MatrixBox[1][4].width + 3, MatrixBox[1][4].pozY - 3, screenWidth, MatrixBox[4][4].pozY + MatrixBox[4][4].height + 3);
        }
        if (verifyButton(ButtonSum_M))
        {
            bar(MatrixBox2[4][1].pozX, MatrixBox2[4][4].pozY + MatrixBox2[4][4].height + 5, screenWidth, ButtonNCol_M.pozY);
            SumButtons = false;
            exitanimateButton(ButtonMult_M);
            exitanimateButton(ButtonSumElem_M);
            clearMatrix(2);
            showMatrixResultSum = true;
            showMatrixResultMult = false;
            showMatrixResultTrans = false;
            animateButton(ButtonSum_M);
            bar(MatrixBox[1][4].pozX + MatrixBox[1][4].width + 4, MatrixBox[1][4].pozY - 4, screenWidth, MatrixBox[4][4].pozY + MatrixBox[4][4].height + 4);
            settextjustify(CENTER_TEXT, CENTER_TEXT);
            outtextxy(MatrixBox[2][4].pozX + MatrixBox[2][4].width + 20, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, "+");
            createMatrixBox2(MatrixBox2[1][1].text, 1, 1);
            showMatrix2 = true;
        }
        if (verifyButton(ButtonMult_M))
        {
            bar(MatrixBox2[4][1].pozX, MatrixBox2[4][4].pozY + MatrixBox2[4][4].height + 5, screenWidth, ButtonNCol_M.pozY);
            SumButtons = false;
            exitanimateButton(ButtonSum_M);
            exitanimateButton(ButtonSumElem_M);
            clearMatrix(2);
            countCol2 = 0;
            showMatrixResultMult = true;
            showMatrixResultSum = false;
            showMatrixResultTrans = false;
            animateButton(ButtonMult_M);
            bar(MatrixBox[1][4].pozX + MatrixBox[1][4].width + 4, MatrixBox[1][4].pozY - 4, screenWidth, MatrixBox[4][4].pozY + MatrixBox[4][4].height + 4);
            settextjustify(CENTER_TEXT, CENTER_TEXT);
            outtextxy(MatrixBox[2][4].pozX + MatrixBox[2][4].width + 20, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, "*");
            createMatrixBox2(MatrixBox2[1][1].text, 1, 1);
            showMatrix2 = true;
        }

        if (verifyButton(ButtonPower_M))
        {
            bar(MatrixBox2[4][1].pozX, MatrixBox2[4][4].pozY + MatrixBox2[4][4].height + 5, screenWidth, ButtonNCol_M.pozY);
            SumButtons = false;
            exitanimateButton(ButtonSum_M);
            exitanimateButton(ButtonSumElem_M);
            clearMatrix(2);
            showMatrixResultMult = false;
            showMatrixResultSum = false;
            showMatrixResultTrans = false;
            animateButton(ButtonPower_M);
            bar(MatrixBox[1][4].pozX + MatrixBox[1][4].width + 4, MatrixBox[1][4].pozY - 4, screenWidth, MatrixBox[4][4].pozY + MatrixBox[4][4].height + 4);
            int n = RequestNumber_V(ButtonPower_M);
            ShowResultPow(n);
            exitanimateButton(ButtonPower_M);
        }

        if (verifyButton(ButtonDet_M))
        {
            bar(MatrixBox2[4][1].pozX, MatrixBox2[4][4].pozY + MatrixBox2[4][4].height + 5, screenWidth, ButtonNCol_M.pozY);
            SumButtons = false;
            exitanimateButton(ButtonMult_M);
            exitanimateButton(ButtonSumElem_M);
            exitanimateButton(ButtonSum_M);

            clearMatrix(2);
            showMatrixResultSum = false;
            showMatrixResultMult = false;

            animateButton(ButtonDet_M);
            bar(MatrixBox[1][4].pozX + MatrixBox[1][4].width + 4, MatrixBox[1][4].pozY - 4, screenWidth, MatrixBox[4][4].pozY + MatrixBox[4][4].height + 4);
            settextjustify(CENTER_TEXT, CENTER_TEXT);
            outtextxy(MatrixBox[2][4].pozX + MatrixBox[2][4].width + 20, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, "=");
            ShowResultDet();
            exitanimateButton(ButtonDet_M);
        }

        if (verifyButton(ButtonTrans_M))
        {
            bar(MatrixBox2[4][1].pozX, MatrixBox2[4][4].pozY + MatrixBox2[4][4].height + 5, screenWidth, ButtonNCol_M.pozY);
            SumButtons = false;
            exitanimateButton(ButtonMult_M);
            exitanimateButton(ButtonSumElem_M);
            exitanimateButton(ButtonSum_M);

            clearMatrix(2);
            showMatrixResultTrans = true;
            showMatrixResultSum = false;
            showMatrixResultMult = false;

            animateButton(ButtonTrans_M);
            bar(MatrixBox[1][4].pozX + MatrixBox[1][4].width + 4, MatrixBox[1][4].pozY - 4, screenWidth, MatrixBox[4][4].pozY + MatrixBox[4][4].height + 4);
            settextjustify(CENTER_TEXT, CENTER_TEXT);
            outtextxy(MatrixBox[2][4].pozX + MatrixBox[2][4].width + 20, MatrixBox[2][4].pozY + MatrixBox[2][4].height/2, "->");
            showMatrix2 = true;
            ShowResultTrans();
            showMatrix2 = false;
            exitanimateButton(ButtonTrans_M);
        }

        if (verifyButton(ButtonInv_M))
        {
            bar(MatrixBox2[4][1].pozX, MatrixBox2[4][4].pozY + MatrixBox2[4][4].height + 5, screenWidth, ButtonNCol_M.pozY);
            SumButtons = false;
            exitanimateButton(ButtonMult_M);
            exitanimateButton(ButtonSumElem_M);
            exitanimateButton(ButtonSum_M);

            clearMatrix(2);
            showMatrixResultSum = false;
            showMatrixResultMult = false;

            animateButton(ButtonInv_M);
            bar(MatrixBox[1][4].pozX + MatrixBox[1][4].width + 4, MatrixBox[1][4].pozY - 4, screenWidth, MatrixBox[4][4].pozY + MatrixBox[4][4].height + 4);

            ShowResultInv();
            exitanimateButton(ButtonInv_M);
        }

        if (verifyButton(ButtonSumElem_M))
        {
            bar(MatrixBox2[4][1].pozX, MatrixBox2[4][4].pozY + MatrixBox2[4][4].height + 5, screenWidth, ButtonNCol_M.pozY);
            if (countLin == countCol && countLin != 0 && countCol!=0)
            {
                showMatrix2 = false;
                showMatrixResultTrans = false;
                showMatrixResultMult = false;
                showMatrixResultSum = false;
                exitanimateButton(ButtonMult_M);
                exitanimateButton(ButtonSum_M);
                bar(MatrixBox[1][4].pozX + MatrixBox[1][4].width + 3, MatrixBox[1][4].pozY - 3, screenWidth, MatrixBox[4][4].pozY + MatrixBox[4][4].height + 3);
                animateButton(ButtonSumElem_M);
                showButtonSums();
                SumButtons = true;
            }
            else
            {
                settextjustify(LEFT_TEXT, TOP_TEXT);
                setcolor(LIGHTRED);
                outtextxy(ButtonSumElem_M.pozX + ButtonSumElem_M.width + 10, ButtonSumElem_M.pozY + 10, "Matricea nu este patratica!");
                if(settings.playFXSounds) PlaySound(TEXT("error.wav"), NULL, SND_FILENAME|SND_ASYNC);
                settextjustify(CENTER_TEXT, CENTER_TEXT);
                delay(500);
                bar(ButtonSumElem_M.pozX + ButtonSumElem_M.width + 5, ButtonSumElem_M.pozY, screenWidth, ButtonSumElem_M.pozY + ButtonSumElem_M.height);
                setcolor(settings.darkTheme?settings.textColor.dark:settings.textColor.light);
            }
        }

        if (SumButtons)
        {
            if(verifyButton(ButtonSumDiagP))
            {
                createMatrixBox(MatrixBox[1][1].text, 1, 1);
                animateButton(ButtonSumDiagP);
                SumDiagP();
                exitanimateButton(ButtonSumDiagP);
            }

            if(verifyButton(ButtonSumDiagP_U))
            {
                createMatrixBox(MatrixBox[1][1].text, 1, 1);
                animateButton(ButtonSumDiagP_U);
                SumDiagP_U();
                exitanimateButton(ButtonSumDiagP_U);
            }

            if(verifyButton(ButtonSumDiagP_A))
            {
                createMatrixBox(MatrixBox[1][1].text, 1, 1);
                animateButton(ButtonSumDiagP_A);
                SumDiagP_A();
                exitanimateButton(ButtonSumDiagP_A);
            }

            if(verifyButton(ButtonSumDiagS))
            {
                createMatrixBox(MatrixBox[1][1].text, 1, 1);
                animateButton(ButtonSumDiagS);
                SumDiagS();
                exitanimateButton(ButtonSumDiagS);
            }

            if(verifyButton(ButtonSumDiagS_U))
            {
                createMatrixBox(MatrixBox[1][1].text, 1, 1);
                animateButton(ButtonSumDiagS_U);
                SumDiagS_U();
                exitanimateButton(ButtonSumDiagS_U);
            }

            if(verifyButton(ButtonSumDiagS_A))
            {
                createMatrixBox(MatrixBox[1][1].text, 1, 1);
                animateButton(ButtonSumDiagS_A);
                SumDiagS_A();
                exitanimateButton(ButtonSumDiagS_A);
            }
        }

        if (verifyButton(ButtonSavedFile_M))
        {
            if(settings.playFXSounds)
                PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME|SND_ASYNC);
            FilledMatrix = true;
            bar(MatrixBox[1][4].pozX + MatrixBox[1][4].width + 3, MatrixBox[1][4].pozY - 3, screenWidth, MatrixBox[4][4].pozY + MatrixBox[4][4].height + 3);
            animateButton(ButtonSavedFile_M);
            ifstream SavedFileM_in ("savedfileMatrix.txt");
            int valoare;

            SavedFileM_in>>countLin>>countCol;
            clearMatrix(1);
            createMatrixBox(MatrixBox[1][1].text, 1, 1);

            for(int i = 1; i<=countLin; i++)
                for (int j=1; j<=countCol; j++)
                {
                    SavedFileM_in>>valoare;
                    MatrixBox[i][j].value = valoare;
                    NumbertoChar(valoare, functionCall_v);
                    createMatrixBox(functionCall_v, i, j);
                    animateMbox(i,j, settings.darkTheme?settings.highlightColor.dark:settings.highlightColor.light);
                    delay(100);
                    exitanimateMbox(i, j);
                    delay(settings.fastAnim?settings.animationSpeed.fast:settings.animationSpeed.slow);
                }

            NumbertoChar(countLin, functionCall_v);
            bar(ButtonNLin_M.pozX + ButtonNLin_M.width + 4, ButtonNLin_M.pozY - 1, screenWidth, ButtonNLin_M.pozY + ButtonNLin_M.height);
            outtextxy(ButtonNLin_M.pozX + ButtonNLin_M.width + 20, ButtonNLin_M.pozY + 20, functionCall_v);

            NumbertoChar(countCol, functionCall_v);
            bar(ButtonNCol_M.pozX + ButtonNCol_M.width + 4, ButtonNCol_M.pozY - 1, screenWidth, ButtonNCol_M.pozY + ButtonNCol_M.height);
            outtextxy(ButtonNCol_M.pozX + ButtonNCol_M.width + 20, ButtonNCol_M.pozY + 20, functionCall_v);

            exitanimateButton(ButtonSavedFile_M);
        }
    }
}

void mouseTrackerSettings()
{
    getmouseclick(WM_LBUTTONDOWN, mouse.x, mouse.y);
    if(verifyButton(ButtonDarkTheme))
    {
        if(settings.darkTheme)
            settings.darkTheme = false;
        else settings.darkTheme = true;

        StartScreen();

        if (settings.darkTheme)
            readimagefile("check_dark.jpg", ButtonDarkTheme.pozX + ButtonDarkTheme.width + 20, ButtonDarkTheme.pozY, ButtonDarkTheme.pozX + ButtonDarkTheme.width + 60, ButtonDarkTheme.pozY + 40);

        animateButton(ButtonDarkTheme);
        exitanimateButton(ButtonDarkTheme);
    }
    if(verifyButton(ButtonMute))
    {
        if(settings.playFXSounds)
            settings.playFXSounds = false;
        else settings.playFXSounds = true;

        if(!settings.playFXSounds)
        {
            if(settings.darkTheme)
                readimagefile("check_dark.jpg", ButtonMute.pozX + ButtonMute.width + 20, ButtonMute.pozY, ButtonMute.pozX + ButtonMute.width + 60, ButtonMute.pozY + 40);
            else
                readimagefile("check_light.jpg", ButtonMute.pozX + ButtonMute.width + 20, ButtonMute.pozY, ButtonMute.pozX + ButtonMute.width + 60, ButtonMute.pozY + 40);
        }
        else
            bar(ButtonMute.pozX + ButtonMute.width + 10, ButtonMute.pozY, screenWidth, ButtonMute.pozY + 42);


        animateButton(ButtonMute);
        exitanimateButton(ButtonMute);
    }
    if(verifyButton(ButtonSlowAnim))
    {
        if(settings.fastAnim)
            settings.fastAnim = false;
        else settings.fastAnim = true;

        if(!settings.fastAnim)
        {
            if(settings.darkTheme)
                readimagefile("check_dark.jpg", ButtonSlowAnim.pozX + ButtonSlowAnim.width + 20, ButtonSlowAnim.pozY, ButtonSlowAnim.pozX + ButtonSlowAnim.width + 60, ButtonSlowAnim.pozY + 40);
            else
                readimagefile("check_light.jpg", ButtonSlowAnim.pozX + ButtonSlowAnim.width + 20, ButtonSlowAnim.pozY, ButtonSlowAnim.pozX + ButtonSlowAnim.width + 60, ButtonSlowAnim.pozY + 40);
        }

        else
            bar(ButtonSlowAnim.pozX + ButtonSlowAnim.width + 10, ButtonSlowAnim.pozY, screenWidth, ButtonSlowAnim.pozY + 42);

        animateButton(ButtonSlowAnim);
        exitanimateButton(ButtonSlowAnim);
    }
    if(verifyButton(ButtonVector))
    {
        VectorScreen();
        startVector = true;
        startSettings = false;
    }
    if (verifyButton(ButtonMatrix))
    {
        MatrixScreen();
        startMatrix = true;
        startSettings = false;
    }
    if (verifyButton(ButtonInstructions))
    {
        InstructionsScreen();
        startSettings = false;
        startInstructions = true;
    }

    if (verifyButton(ButtonExit))
    {
        startSettings = false;
        Exit = true;
    }

}

void VectorScreen()
{
    bar(ButtonVector.width + 2*ButtonVector.pozX + 12, 1, screenWidth, screenHeight);

    settextstyle(settings.textFont, HORIZ_DIR, 3);
    createVectorBox(VectorBox[1].text, 1);

    showButtonSum_V();
    showButtonMult_V();
    showButtonScalar_V();
    showButtonSavedFile_V();
    showButtonBubbleSort_V();
    showButtonQuickSort_V();
    showButtonShift_R();
    showButtonShift_L();
    showButtonElim_V();
}

void MatrixScreen()
{
    bar(ButtonVector.width + 2*ButtonVector.pozX + 12, 1, screenWidth, screenHeight);

    settextstyle(settings.textFont, HORIZ_DIR, 3);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    createMatrixBox(MatrixBox[1][1].text, 1, 1);

    NumbertoChar(countLin, functionCall_v);
    showButtonNLin_M(), outtextxy(ButtonNLin_M.pozX + ButtonNLin_M.width + 20, ButtonNLin_M.pozY + 20, functionCall_v);
    NumbertoChar(countCol, functionCall_v);
    showButtonNCol_M(), outtextxy(ButtonNCol_M.pozX + ButtonNCol_M.width + 20, ButtonNCol_M.pozY + 20, functionCall_v);
    showButtonSum_M();
    showButtonMult_M();
    showButtonPower_M();
    showButtonDet_M();
    showButtonTrans_M();
    showButtonInv_M();
    showButtonSumElem_M();
    showButtonSavedFile_M();
}

void SettingsScreen()
{
    bar(ButtonVector.width + 2*ButtonVector.pozX + 12, 1, screenWidth, screenHeight);
    settextstyle(settings.textFont, HORIZ_DIR, 5);
    settextjustify(LEFT_TEXT, CENTER_TEXT);
    outtextxy(screenWidth/3, 65, "Setari");
    settextstyle(settings.textFont, HORIZ_DIR, 3);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    showButtonDarkTheme();
    showButtonMute();
    showButtonSlowAnim();

    if (settings.darkTheme)
        readimagefile("check_dark.jpg", ButtonDarkTheme.pozX + ButtonDarkTheme.width + 20, ButtonDarkTheme.pozY, ButtonDarkTheme.pozX + ButtonDarkTheme.width + 60, ButtonDarkTheme.pozY + 40);

    if(!settings.playFXSounds)
    {
        if(settings.darkTheme)
            readimagefile("check_dark.jpg", ButtonMute.pozX + ButtonMute.width + 20, ButtonMute.pozY, ButtonMute.pozX + ButtonMute.width + 60, ButtonMute.pozY + 40);
        else
            readimagefile("check_light.jpg", ButtonMute.pozX + ButtonMute.width + 20, ButtonMute.pozY, ButtonMute.pozX + ButtonMute.width + 60, ButtonMute.pozY + 40);
    }

    if(!settings.fastAnim)
    {
        if(settings.darkTheme)
            readimagefile("check_dark.jpg", ButtonSlowAnim.pozX + ButtonSlowAnim.width + 20, ButtonSlowAnim.pozY, ButtonSlowAnim.pozX + ButtonSlowAnim.width + 60, ButtonSlowAnim.pozY + 40);
        else
            readimagefile("check_light.jpg", ButtonSlowAnim.pozX + ButtonSlowAnim.width + 20, ButtonSlowAnim.pozY, ButtonSlowAnim.pozX + ButtonSlowAnim.width + 60, ButtonSlowAnim.pozY + 40);
    }
}

void InstructionsScreen()
{
    bar(ButtonVector.width + 2*ButtonVector.pozX + 12, 1, screenWidth, screenHeight);
    settextjustify(LEFT_TEXT, CENTER_TEXT);
    settextstyle(settings.textFont, HORIZ_DIR, 5);
    outtextxy(screenWidth/3, 65, "Instructiuni");
    settextstyle(settings.textFont, HORIZ_DIR, 2);
    outtextxy(screenWidth/3, 200, "Programul prezinta grafic executarea mai multor operatii pe tablouri");
    outtextxy(screenWidth/3, 220, "unidimensionale si bidimensionale.");
    outtextxy(screenWidth/3, 270, "Vectori:");
    outtextxy(screenWidth/3, 290, "Elementele vectorului sunt indexate de la 1. Pentru a adauga un element in vector");
    outtextxy(screenWidth/3, 310, "veti apasa pe cea mai din dreapta casuta libera.");
    outtextxy(screenWidth/3, 360, "Matrici:");
    outtextxy(screenWidth/3, 380, "Pentru a putea adauga elemente in matrice, va trebui sa introduceti intai numarul");
    outtextxy(screenWidth/3, 400, "de linii si de coloane dorit (cuprins intre 1 si 4).");
    outtextxy(screenWidth/3, 450, "Informatii generale:");
    outtextxy(screenWidth/3, 470, "Puteti introduce numere intregi, atat pozitive, cat si negative. Puteti modifica");
    outtextxy(screenWidth/3, 490, "elementele deja introduse. Dupa adaugarea sau modificarea unui element");
    outtextxy(screenWidth/3, 510, "veti apasa tasta enter.");
    settextjustify(CENTER_TEXT, CENTER_TEXT);
}

void mouseTrackerInstructions()
{
    getmouseclick(WM_LBUTTONDOWN, mouse.x, mouse.y);

    if(verifyButton(ButtonVector))
    {
        VectorScreen();
        startVector = true;
        startInstructions = false;
    }
    if (verifyButton(ButtonMatrix))
    {
        MatrixScreen();
        startMatrix = true;
        startInstructions = false;
    }
    if (verifyButton(ButtonSettings))
    {
        SettingsScreen();
        startSettings = true;
        startInstructions = false;
    }
    if (verifyButton(ButtonExit))
    {
        startInstructions = false;
        Exit = true;
    }

}

void mouseTrackerStart()
{
    if (hoverButton(ButtonVector))
        VectorButtonAnimation();
    else ExitVectorButtonAnimation();

    if (hoverButton(ButtonMatrix))
        MatrixButtonAnimation();
    else ExitMatrixButtonAnimation();

    if (hoverButton(ButtonInstructions))
        InstructionsButtonAnimation();
    else ExitInstructionsButtonAnimation();

    if (hoverButton(ButtonSettings))
        SettingsButtonAnimation();
    else ExitSettingsButtonAnimation();

    if (hoverButton(ButtonExit))
        ExitButtonAnimation();
    else ExitExitButtonAnimation();

    if(ismouseclick(WM_LBUTTONDOWN) && startVector)
    {
        mouseTrackerVector();
    }
    else if(ismouseclick(WM_LBUTTONDOWN) && startMatrix)
    {
        mouseTrackerMatrix();
    }
    else if (ismouseclick(WM_LBUTTONDOWN) && startInstructions)
    {
        mouseTrackerInstructions();
    }
    else if(ismouseclick(WM_LBUTTONDOWN) && startSettings)
    {
        mouseTrackerSettings();
    }

    else if (ismouseclick(WM_LBUTTONDOWN))
    {
        getmouseclick(WM_LBUTTONDOWN, mouse.x, mouse.y);
        if (verifyButton(ButtonVector))
        {
            VectorScreen();
            mouseTrackerVector();
            startMatrix = false;
            startInstructions = false;
            startVector = true;
            startSettings = false;
        }

        if (verifyButton(ButtonMatrix))
        {
            MatrixScreen();
            mouseTrackerMatrix();
            startVector = false;
            startInstructions = false;
            startMatrix = true;
            startSettings = false;
        }

        if (verifyButton(ButtonInstructions))
        {
            InstructionsScreen();
            mouseTrackerInstructions();
            startVector = false;
            startMatrix = false;
            startSettings = false;
            startInstructions = true;
        }

        if (verifyButton(ButtonSettings))
        {
            SettingsScreen();
            mouseTrackerSettings();
            startVector = false;
            startMatrix = false;
            startInstructions = false;
            startSettings = true;
        }

        if (verifyButton(ButtonExit))
        {
            Exit = true;
        }
    }
}

void initstruct() //initializarea structurilor
{
    settextstyle(settings.textFont, HORIZ_DIR, 3);
    for (int i = 1; i<=1000; i++)
        VectorBox[i].width = textwidth("_") + 20;

    int poz = 65;
    for (int j=1; j<=4; j++)
        MatrixBox[1][j].pozY = poz;
    for (int i=2; i<=4; i++)
    {
        for (int j=1; j<=4; j++)
            MatrixBox[i][j].pozY = MatrixBox[i-1][j].pozY + MatrixBox[i-1][j].height + 10;
    }
    for (int i=1; i<=4; i++)
        for (int j=1; j<=4; j++)
        {
            if(j!=1)
                MatrixBox[i][j].pozX = MatrixBox[i][j-1].pozX + MatrixBox[i][j-1].width + 10;
        }

    for (int j=1; j<=4; j++)
        MatrixBox2[1][j].pozY = poz;
    for (int i=2; i<=4; i++)
    {
        for (int j=1; j<=4; j++)
            MatrixBox2[i][j].pozY = MatrixBox2[i-1][j].pozY + MatrixBox2[i-1][j].height + 10;
    }

    for (int i=1; i<=4; i++)
        MatrixBox2[i][1].pozX = 800;
    for (int i=1; i<=4; i++)
        for (int j=1; j<=4; j++)
        {
            if(j!=1)
                MatrixBox2[i][j].pozX = MatrixBox2[i][j-1].pozX + MatrixBox2[i][j-1].width + 10;
        }

    for (int j=1; j<=4; j++)
        MatrixBoxResult[1][j].pozY = poz;
    for (int i=2; i<=4; i++)
    {
        for (int j=1; j<=4; j++)
            MatrixBoxResult[i][j].pozY = MatrixBoxResult[i-1][j].pozY + MatrixBoxResult[i-1][j].height + 10;
    }
    for (int i=1; i<=4; i++)
        MatrixBoxResult[i][1].pozX = 1150;
    for (int i=1; i<=4; i++)
        for (int j=1; j<=4; j++)
        {
            if(j!=1)
                MatrixBoxResult[i][j].pozX = MatrixBoxResult[i][j-1].pozX + MatrixBoxResult[i][j-1].width + 10;
        }

    VectorBox[1].show = true;
    setlinestyle(4, 0, 3);
}

int main()
{
    ifstream SavedFileSettings_in ("savedfileSettings.txt");
    SavedFileSettings_in>>settings.darkTheme>>settings.playFXSounds>>settings.fastAnim; //actualizam setarile utilizand datele din fisier

    if(settings.playFXSounds)
        PlaySound(TEXT("pornire.wav"), NULL, SND_FILENAME|SND_ASYNC);

    initwindow(screenWidth, screenHeight, "VectMat", false, false);
    initstruct();

    StartScreen();
    while(Running)
    {
        mouseTrackerStart();
        if(Exit)
            closegraph(), Running = false;
    }

    ofstream SavedFileVector_out ("savedfileVector.txt");
    ofstream SavedFileMatrix_out ("savedfileMatrix.txt");
    ofstream SavedFileSettings_out ("savedfileSettings.txt");

    SavedFileVector_out<<countV<<'\n';
    for (int i = 1; i<=countV; i++)
        SavedFileVector_out<<VectorBox[i].value<<' ';

    SavedFileMatrix_out<<countLin<<'\n';
    SavedFileMatrix_out<<countCol<<'\n';
    for (int i = 1; i<=countLin; i++)
    {
        for (int j = 1; j<=countCol; j++)
            SavedFileMatrix_out<<MatrixBox[i][j].value<<' ';
        SavedFileMatrix_out<<'\n';
    }
    SavedFileSettings_out<<settings.darkTheme<<' '<<settings.playFXSounds<<' '<<settings.fastAnim;

    return 0;
}
