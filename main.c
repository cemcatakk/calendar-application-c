#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<time.h>
#include<windows.h>
#include<dos.h>
#include <conio.h> 
int eventID;
struct Date{
    int d;
    int m;
    int y;
};
struct Date date;
struct _TIME {
  int hour;
  int minute;

}ttime;

typedef struct _EVENT {
   char short_name[32];
   char description[100];
   struct _TIME ttime;
   int d;
   int m;
   int y;
   int id;
   int alert;
   struct _EVENT *next;
}tevent;
tevent *list = NULL;
tevent *templist = NULL;
//Iki saati karsilastirip, esitlik ya da buyuluk durumuna gore 0,1 ve -1 degeri donduren metot
int compareTimes(struct _TIME t1,struct _TIME t2){
	if(t1.hour==t2.hour){
		if(t1.minute>t2.minute)
			return 1;
		else if(t1.minute<t2.minute)
			return -1;
		else return 0;
	}
	else if(t1.hour>t2.hour)return 1;
	else return -1;
}
//events.txt dosyasindan belli formatta verileri okuyan metot
void readEvents(){
	list=NULL;
	FILE* f = fopen("events.txt","r");
	if(!f)exit(-1);
	int id;
	while(!feof(f)){
		fscanf(f,"%d\n",&id);
		if (feof(f)) break;
		tevent *event = (tevent *)malloc(sizeof(tevent));
		fgets(event->short_name,32,f);
		event->short_name[strlen(event->short_name)-1]=0;
		fgets(event->description,99,f);
		event->description[strlen(event->description)-1]=0;
		fscanf(f,"%d:%d:%d\n%d:%d\n%d\n",&event->d,&event->m,&event->y
		,&event->ttime.hour,&event->ttime.minute,&event->alert);
		event->id=eventID;
		eventID++;
		//Burada listeye ekleme yapilirken saate gore sirali bir sekilde ekleme yapiliyor
		//20:20->20:30->20:40 gibi
		if(strcmp(event->short_name,"")==0)eventID--;
		else{
			if (list == NULL||compareTimes(list->ttime,event->ttime)==1)
	    {
	        event->next = list;
	        list = event;
	    }
	    else{
	    	tevent* tmpd = list;
		    while (tmpd->next != NULL && compareTimes(tmpd->next->ttime,event->ttime)==-1){
		        tmpd = tmpd->next;
		    }
		    event->next = tmpd->next;
		    tmpd->next = event;
		}
		}
	}
	printf("Events readed from database.\n");
	fclose(f);
}
//Listedeki veriler events.txt dosyasina yaziliyor
void writeEvents(){
	tevent* event = list;
	FILE* f = fopen("events.txt","w");
	while(event!=NULL){
		fprintf(f,"%d\n%s\n%s\n%d:%d:%d\n%d:%d\n%d\n",event->id,event->short_name,event->description,event->d,event->m,event->y
		,event->ttime.hour,event->ttime.minute,event->alert);
		event=event->next;
	}
	fclose(f);
	printf("Events saved to database.\n");
}
//Evenet yapisi uygun formatta ekrana yazdiriliyor
void printEvent(tevent* event){
	printf("%-14s %-14d\n","Event ID:",event->id);
	printf("%-14s %-40s\n","Event Name:",event->short_name);
	printf("%-14s %-99s\n","Event Description:",event->description);
	printf("%-14s %d-%d-%d %d:%d\n","Event Date:",event->d,event->m,event->y,event->ttime.hour,event->ttime.minute);
	printf("%-14s %-40d\n","Alert:",event->alert);
}
//Verilen tarih, mevcut zamanla karsilastiriliyor, kucuk ise 1, degilse 0 donduruluyor
int checkdate(int d,int m,int y,int h,int min){
	time_t t = time(NULL);
	struct tm dt = *localtime(&t);
	int cury = dt.tm_year+1900;
	int curm = dt.tm_mon+1;
	if(cury>y){
		return 1;
	}
	else if(cury==y){
		if(curm>m){
			return 1;
		}
		else if(curm==m){
			 if(dt.tm_mday>d){
				return 1;
			}
			else if(dt.tm_mday==d){
				if(dt.tm_hour>h){
					return 1;
				}
				else if(dt.tm_hour==h){
					if(dt.tm_min>=min){
						return 1;
					}
				}
			}
		}
	}
	return 0;
}
//Sistemde kayitli alarmlar kontrol edilip, calinmasi gerekenler caliniyor
void checkAlerts(){
	tevent* event = list;
	while(event!=NULL){
		if(event->alert==1){
			if(checkdate(event->d,event->m,event->y,event->ttime.hour,event->ttime.minute)==1){
				printf("\a");
				printf("*****ALERT FOR %s EVENT*****\n",event->short_name);
				printEvent(event);
				event->alert=0;
			}
		}
		event=event->next;
	}
}
//tarih bilgisi parametre ile veriliyor ve bu tarihe ait veriler otomatik olarak saate gore azalan sirada yazdiriliyor
void checkEvents(int d,int m,int y){
	tevent* event = list;
	printf("\n*****EVENTS FOUND*****\n\n");
	while(event!=NULL){
		if(event->d==d&&event->m==m&&event->y==y){
		 	printEvent(event);printf("---------------------\n");
		}
		event=event->next;
	}
	printf("\n");
}
//Tarih bilgisi alinip bu tarihte etkinlik olup olmadigi bilgisi kontrol ediliyor
int checkEventExists(int d,int m,int y){
	tevent* event = list;
	while(event!=NULL){
		if(event->d==d&&event->m==m&&event->y==y){
			return 1;
		}
		event=event->next;
	}
	return -1;
}
//Yeni etkinlik eklenen method
void addevent(char *short_name, char *descripton,int d,int m,int y, struct _TIME event_time,int alert)
{
    tevent *event = (tevent *)malloc(sizeof(tevent));
    if (event == NULL)
    {
        return;
    }
    strncpy(event->short_name, short_name, sizeof(event->short_name));
    strncpy(event->description, descripton, sizeof(event->description));
    event->id = eventID;
    event->d = d;
    event->m = m;
    event->y = y;
    event->alert=alert;
   (*event).ttime.hour = event_time.hour;
   (*event).ttime.minute = event_time.minute;
   //Sirali ekleme islemi burada da uygulaniyor
	if (list == NULL||compareTimes(list->ttime,event->ttime)==1)
    {
        event->next = list;
        list = event;
    }
    else{
    	  tevent* tmpd = list;
	    while (tmpd->next != NULL && compareTimes(tmpd->next->ttime,event->ttime)==-1){
	        tmpd = tmpd->next;
	    }
	    event->next = tmpd->next;
	    tmpd->next = event;
	}
    eventID++;
}
void editevent(int id)
{
	//verilen ID degeri bulunursa, bu etkinligin bilgileri kullanicidan tekrar aliniyor ve kaydediliyor
    tevent *event = list;
    while(event!=NULL)
    {
        if (event->id == id)
        {
        	printf("Event found, enter new values\n");
            fflush(stdin);
             printf("Enter a short name for the event:");
             fgets(event->short_name,32,stdin);
             event->short_name[strlen(event->short_name)-1]=0;
             printf("Enter the description of the event:");
             fgets(event->description,99,stdin);
             event->description[strlen(event->description)-1]=0;
             printf("enter the date of the event day-month-year \n");
             scanf("%d-%d-%d",&event->d,&event->m,&event->y);
             printf("Enter a time expressed as hh:mm format.\n");
             scanf("%02d:%02d",&event->ttime.hour, &event->ttime.minute);
             printf("Do you want to set an alarm for the event Y/N \n");
             char alarm[4];
             scanf("%s",alarm);
             if(strcmp(alarm,"y")==0||strcmp(alarm,"Y")==0){
                event->alert=1;
             }
             else{
             	event->alert=0;
                break;
             }
            break;
        }
        event = event->next;
    }
}
void printAll(){
	if(list==NULL){
		printf("No events found.\n");
		return;
	}
    tevent *event = list;
    while(event!=NULL){
		printEvent(event);printf("---------------------\n");
    	event=event->next;
	}
	event=list;
}
void deleteEvent(int id)
{
    tevent *event = list;
    tevent *prev = NULL;
    while(event!=NULL)
    {
        if (event->id == id)
        {
            if (prev)
                prev->next = event->next;
            else
                list = event->next;
            free(event);
            break;
        }
        prev = event;
        event = event->next;
    }
}
int LeapYear(int year){ //checks whether the year passed is leap year or not
    if((year % 100!=0 && year % 4 ==0) || year % 400 == 0 )
       return 1;
    return 0;
}
int Number_Days(int month,int year){ //returns the number of days in given month
   switch(month){                          //and year
      case 1 :
           return(31);
      case 2 :
         if(LeapYear(year)==1)
		 return(29);
	       else
		 return(28);
      case 3 :
          return(31);
      case 4 :
           return(30);
      case 5 :
           return(31);
      case 6 :
          return(30);
      case 7 :
           return(31);
      case 8 :
           return(31);
      case 9 :
           return(30);
      case 10:
           return(31);
      case 11:
           return(30);
      case 12:
           return(31);
   }
}

char *nameOFday(int day){ //returns the name of the day
   switch(day){
      case 0 :
          return("Sunday");
      case 1 :
          return("Monday");
      case 2 :
          return("Tuesday");
      case 3 :
          return("Wednesday");
      case 4 :
          return("Thursday");
      case 5 :
          return("Friday");
      case 6 :
          return("Saturday");
   }
}

void print_date(int month, int year){ //prints the name of month and year
    printf("\n\n");
	switch(month){
        case 1:
             printf("%10s","January");
             break;
        case 2:
             printf("%10s","February");
            break;
        case 3:
            printf("%10s","March");
            break;
        case 4:
            printf("%10s","April");
            break;
        case 5:
             printf("%10s","May");
             break;
        case 6:
             printf("%10s","June");
             break;
        case 7:
            printf("%10s","July");
            break;
        case 8:
            printf("%10s","August");
            break;
        case 9:
            printf("%10s","September");
            break;
        case 10:
             printf("%10s","October");
             break;
        case 11:
            printf("%10s","November");
            break;
        case 12:
            printf("%10s","December");
            break;
    }
    printf(" %d\n", year);
}

int getDayNumber(int day,int mon,int year){ //returns the day number
    int res = 0, t1, t2, y = year;
    year = year - 1600;
    while(year >= 100){
        res = res + 5;
        year = year - 100;
    }
    res = (res % 7);
    t1 = ((year - 1) / 4);
    t2 = (year-1)-t1;
    t1 = (t1*2)+t2;
    t1 = (t1%7);
    res = res + t1;
    res = res%7;
    t2 = 0;
    for(t1 = 1;t1 < mon; t1++){
        t2 += Number_Days(t1,y);
    }
    t2 = t2 + day;
    t2 = t2 % 7;
    res = res + t2;
    res = res % 7;
    if(y > 2000)
        res = res + 1;
    res = res % 7;
    return res;
}
    char *getDay(int d,int m,int y){
    int day;
    if(y>=1600){
        day = getDayNumber(d,m,y);
        day = day%7;
        return(nameOFday(day));
    }
}

void printMonth(int mon,int year){ //prints the month with all days
    int nod, day, cnt, d = 1, isNote = 0,i;
    print_date(mon,year);
    printf("%3s%3s%3s%3s%3s%3s%3s\n","S","M","T","W","T","F","S");
    nod = Number_Days(mon,year);
    day = getDayNumber(d,mon,year);
    cnt=0;
    day = getDayNumber(1,mon,year);
    for(i=0;i<day;i++)printf("%3s"," ");
    for(d=1;d<=nod;d++){
        while(getDayNumber(d,mon,year)!=cnt){
        	cnt++;
        	if(cnt%7==0){
        		cnt=0;
        		printf("\n");
			}
		}
        if(checkEventExists(d,mon,year)==1){
        	printf("%2d*",d);
		}
		else printf("%3d",d);
    }
}

int main()
{
	readEvents();
	eventID=0;	
   int choice;
   char short_name[32];
   char description[100];
   struct _TIME event_time;
   int id;
   int d;
   int m;
   int y;
   int alert;
   char alarm[4];
   char temp[100];
   char quit ;
   while(1)
   {
      system("cls");
      checkAlerts();
      printf("-------------------------\n");
      printf("1. Add a new event \n");
      printf("2. Delete an event\n");
      printf("3. Edit an event\n");
      printf("4. Day view\n");
      printf("5. Month view\n");
      printf("-------------------------\n");
      printf("Enter your choice:");
      scanf(" %c", &choice);
      switch (choice)
      {
          	 case '1':
      		 fflush(stdin);
             printf("Enter a short name for the event:");
             fgets(short_name,32,stdin);
             short_name[strlen(short_name)-1]=0;
             printf("Enter the description of the event:");
             fgets(description,99,stdin);
             description[strlen(description)-1]=0;
             printf("enter the date of the event day-month-year \n");
             scanf("%d-%d-%d",&d,&m,&y);
             printf("Enter a time expressed as hh:mm format.\n");
             scanf("%02d:%02d",&event_time.hour, &event_time.minute);
             printf("Do you want to set an alarm for the event Y/N \n");
             scanf("%s",alarm);
             if(strcmp(alarm,"y")==0||strcmp(alarm,"Y")==0){
                alert=1;
             }
             else{
             	alert=0;
             }
             addevent(short_name,description,d,m,y,event_time,alert);
             break;

             case '2':
             printAll();
             printf("Enter event's id:\n");
             scanf("%d", &id);
             system("cls");
             deleteEvent(id);
             break;

             case '3':
             printAll();
             printf("Enter event's id:");
             scanf("%d", &id);
             system("cls");
             editevent(id);
             getch();
             break;

             case '4':
             printf("Enter date (DD MM YYYY) : ");
             scanf("%d %d %d",&date.d,&date.m,&date.y);
             printf("Day is : %s",getDay(date.d,date.m,date.y));
             printf("\nChecking for events..\n");
             checkEvents(date.d,date.m,date.y);
             scanf(" %c", &choice);
             break;

             case '5':
             printf("Enter month and year (MM YYYY) : ");
             scanf("%d %d",&date.m,&date.y);
             system("cls");
             printMonth(date.m,date.y);
             printf("\n");
             scanf(" %c", &choice);
             break;
         }
         if(choice=='Q'||choice=='q'){
         	char c2;
         	printf("Are you sure? (y/n):");
         	scanf(" %c",&c2);
         	if(c2=='y'||c2=='Y')break;
		 }
    printf("\n");
   }
   writeEvents();
   return 0;
}



