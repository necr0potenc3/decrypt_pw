#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "resource.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow);
BOOL CALLBACK MainDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam);
void NumOut(char *format, ...);
bool crypt_pw(BYTE *in, BYTE *out, int len); //returns 1 if encrypted
bool decrypt_pw(BYTE *in, BYTE *out, int len); //returns 1 if decrypted


HWND myHwnd=0;
BYTE pwdcrypt[0xff];
BYTE pwddecrypt[0xff];

#pragma comment(lib, "comctl32")

void NumOut(char *format, ...)
{
   char final[4096]; memset(final, 0, sizeof(final));
   va_list args;
   va_start(args, format);
   vsprintf(final, format, args);
   va_end(args);
   MessageBox(0, final, "NumOut:", 0);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	//if commctrls isn't initialized dialogbox
	//will return an error cause of ListCtrl
	InitCommonControls();
    
	//create a dialogbox (it makes all the createwindowex job for us)
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOGBAR), 0, MainDlgProc);

	return 0;
}

BOOL CALLBACK MainDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	switch(wMsg)
	{
	case WM_INITDIALOG:
		myHwnd = hDlg;

		strcpy((char*)pwddecrypt, "output here");
		SetDlgItemText(myHwnd, IDC_PWDDE, (LPSTR)pwddecrypt);
		strcpy((char*)pwdcrypt, "input (en/crypted pwd) here");
		SetDlgItemText(myHwnd, IDC_PWD, (LPSTR)pwdcrypt);
		return TRUE;
	case WM_CLOSE:
		EndDialog(myHwnd,0);	// click at cancel-button
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))	// msg is kept in wParam
		{
		case IDC_CRYPT:
		{
			memset(pwdcrypt, 0, 255);
			memset(pwddecrypt, 0, 255);
			GetDlgItemText(myHwnd, IDC_PWD, (LPSTR)pwdcrypt, 255);

			/*builds the log.txt with all the bytes from 0-256 encrypted or not
			BYTE test[256];
			BYTE out[256];
			memset(test, 0, 256);
			memset(out, 0, 256);
			FILE *fp = fopen("log.txt", "w+");
			fprintf(fp, "Encrypted Hex\tDec\tChar\t\t\tNotEncrypted Hex\tDec\tChar\n");
			for(int i=0; i<256; i++)
			{
				test[i] = i;
				crypt_pw(&test[i], &out[i], 1);
				fprintf(fp, "%.3X\t%.3d\t%c\t\t\t%.3X\t%.3d\t%c\n", out[i], out[i], isprint(out[i]) ? out[i] : '.', test[i], test[i], isprint(test[i]) ? test[i] : '.');
			}
			fclose(fp);*/

			crypt_pw(pwdcrypt, pwddecrypt, strlen((const char*)pwdcrypt));

            SetDlgItemText(myHwnd, IDC_PWDDE, (LPSTR)pwddecrypt);
			return TRUE;
		}
		case IDC_DECRYPT:
		{
			memset(pwdcrypt, 0, 255);
			memset(pwddecrypt, 0, 255);
			GetDlgItemText(myHwnd, IDC_PWD, (LPSTR)pwdcrypt, 255);

			decrypt_pw(pwdcrypt, pwddecrypt, strlen((const char*)pwdcrypt));

			SetDlgItemText(myHwnd, IDC_PWDDE, (LPSTR)pwddecrypt);
			return TRUE;
		}
		case IDC_ABOUT:
		{
			NumOut("Created by n0p3, encryption and decryption algorythim made by myself. I'm not canadian. PS: The password is the one stored in uo.cfg");
			return TRUE;
		}
		}
	}
	return FALSE;				// if I don't handle it, pass by blank
}

//shit... I found uo's routine on accident
//but I did this one first and... THEY ARE IDENTICAL! lol
//so this is really uo's method
//my encryption routine based on the decryption routine :)
/*
0047E820  /$ 56             PUSH ESI
0047E821  |. 8B7424 08      MOV ESI,DWORD PTR SS:[ESP+8]
0047E825  |. 57             PUSH EDI
0047E826  |. 8BFE           MOV EDI,ESI
0047E828  |. 83C9 FF        OR ECX,FFFFFFFF
0047E82B  |. 33C0           XOR EAX,EAX
0047E82D  |. F2:AE          REPNE SCAS BYTE PTR ES:[EDI]
0047E82F  |. F7D1           NOT ECX
0047E831  |. 49             DEC ECX
0047E832  |. 85C9           TEST ECX,ECX
0047E834  |. 7E 19          JLE SHORT Client.0047E84F
0047E836  |> 8A1430         /MOV DL,BYTE PTR DS:[EAX+ESI]
0047E839  |. 80C2 0D        |ADD DL,0D
0047E83C  |. 80FA 7F        |CMP DL,7F
0047E83F  |. 881430         |MOV BYTE PTR DS:[EAX+ESI],DL
0047E842  |. 76 06          |JBE SHORT Client.0047E84A
0047E844  |. 80EA 5F        |SUB DL,5F
0047E847  |. 881430         |MOV BYTE PTR DS:[EAX+ESI],DL
0047E84A  |> 40             |INC EAX
0047E84B  |. 3BC1           |CMP EAX,ECX
0047E84D  |.^7C E7          \JL SHORT Client.0047E836
0047E84F  |> 5F             POP EDI
0047E850  |. 5E             POP ESI
0047E851  \. C3             RETN
*/
bool crypt_pw(BYTE *in, BYTE *out, int len)
{
	for(int i=0; i<len; i++)
	{
		BYTE chr = in[i];
		chr += 0x0D; //0x100 - 0xf3
		if(chr > 0x7f) //0x5f + 0x20
			chr -= 0x5f;
		out[i] = chr;
	}
	return 1;
}

//THIS is the UO client way
//found at 200g t2a
/*
0047E860  /$ 56             PUSH ESI
0047E861  |. 8B7424 08      MOV ESI,DWORD PTR SS:[ESP+8]
0047E865  |. 57             PUSH EDI
0047E866  |. 8BFE           MOV EDI,ESI
0047E868  |. 83C9 FF        OR ECX,FFFFFFFF
0047E86B  |. 33C0           XOR EAX,EAX
0047E86D  |. F2:AE          REPNE SCAS BYTE PTR ES:[EDI]
0047E86F  |. F7D1           NOT ECX
0047E871  |. 49             DEC ECX
0047E872  |. 85C9           TEST ECX,ECX
0047E874  |. 7E 19          JLE SHORT Client.0047E88F
0047E876  |> 8A1430         /MOV DL,BYTE PTR DS:[EAX+ESI]
0047E879  |. 80C2 F3        |ADD DL,0F3
0047E87C  |. 80FA 20        |CMP DL,20
0047E87F  |. 881430         |MOV BYTE PTR DS:[EAX+ESI],DL
0047E882  |. 73 06          |JNB SHORT Client.0047E88A
0047E884  |. 80C2 5F        |ADD DL,5F
0047E887  |. 881430         |MOV BYTE PTR DS:[EAX+ESI],DL
0047E88A  |> 40             |INC EAX
0047E88B  |. 3BC1           |CMP EAX,ECX
0047E88D  |.^7C E7          \JL SHORT Client.0047E876
0047E88F  |> 5F             POP EDI
0047E890  |. 5E             POP ESI
0047E891  \. C3             RETN
*/
bool decrypt_pw(BYTE *in, BYTE *out, int len)
{
	for(int i=0; i<len; i++)
	{
		BYTE chr = in[i];
		chr += 0xf3;
		if(chr < 0x20) //jnb
			chr += 0x5f;
		out[i] = chr;
	}
	return 1;
}

//mamaich's encryption routine
bool mamaichcrypt_pw(BYTE *in, BYTE *out, int len)
{
	//UO pwd can't be bigger than 30 chars
	//but fuck it, you know what you do
	if(len > 30)
		NumOut("Are you sure that password is ok? It's too long if you ask me...");

	for(int i=0; i<len; i++)
	{
		int c = in[i];
		//if(c < 31 && c > 127) //32 to 126 are the supported vallues
		//{
		//	strcpy((char*)out, "That password can't be encrypted");
		//	NumOut("Failed: Hex: 0x.2%X Character: %c", c, c);
		//	return 0;
		//}
		c += 13;
		if (c > 126) c -= 95;
		if(c == 32) c = 127;
		//just to debug
		//fprintf(fp, "%.2X\t%.2X\t%.3d\t%c\n", c,i,i, isprint(i) ? i : '.');
		out[i] = c;
	}

	return 1;
}


//the other method I invented, quited fucked up I should say
//shouldnt be used btw, works with mamaich's encryption
bool olddecrypt_pw(BYTE *in, BYTE *out, int len)
{
	if(len > 30)
		NumOut("Are you sure that password is ok? It's too long if you ask me...");

	//could use this one too, it's just cuter and better :)
	for(int i=0; i<len; i++)
	{
		int c = in[i];
		//there's no need to check to see if the encrypted byte
		//is in the encrypted bytes range (0x21 to 0x7f)
		//cause I'll check (at the end of the code) to see
		//if it's in the not-encrypted range
		if(c > 0x20 && c < 0x2D) //between 0x21 and 0x2c
		{
			if(c + 95 > 126) c +=95; //or just xor c, 0x52
			c -= 13;
		}
		else if(c > 0x2c && c < 0x80) //between 0x2d and 0x7f
		{
			if(c == 127) c == 32 + 13 + 13; //0x20
			c -= 13; //could just do xor c, 0x0d
		}
		//check to see if it's in the non-encrypted range
		if(c < 31 && c > 127)
		{
			strcpy((char*)out, "That password can't be broken");
			return 0;
		}
		out[i] = c;
	}

	return 1;
}