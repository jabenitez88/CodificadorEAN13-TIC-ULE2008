/* ***************************** Codificador ISBN - EAN13 ***********************************

  PRACTICA : Codificador de ISBN - EAN13.

  ASIGNATURA : TEORÍA DE LA INFORMACIÓN Y CÓDIGOS (TIC)

  DESARROLLADO POR : JOSE ALBERTO BENÍTEZ ANDRADES

  DOCUMENTACIÓN CODIFICADOR EN : http://pepelife.es/Uni/TIC/codificador.pdf
  
  CÓDIGO EN : http://pepelife.es/Uni/TIC/ISBNCodifierJABA.c

  E-MAIL : infjab02@estudiantes.unileon.es                                                                  

 ***************************************************************************************** */
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk/gdkpixbuf.h>
#include <gdk/gdk.h>
#include <string.h>

#define ancho 800
#define alto 600
#define ancho_cod 780
#define alto_cod 300


#define CORDYINICIAL 200
#define CORDYFINAL 100
#define INICIO 300
#define CENTRO 393
#define FIN 483

// Dibuja una ventana de diálogo de error.
void CodigoErroneo();
// Pinta el rectángulo en el programa principal
void DibujarArea(GtkWidget *widget);
// Dibuja el código de barras una vez obtenido el EAN13 (unos y ceros)
void DibujarCodBarras(GtkWidget *widget);
// Cálculo del código EAN13 a partir del ISBN
void calculaEAN13(GtkWidget *wid);
// Cálculo del código ISBN a partir del EAN13
void calculaISBN(GtkWidget *wid);

/* Los siguientes arrays almacenan la conversión de números decimales del ISBN a su forma binaria.
Para codificar los primeros 6 números, se mirará en el array EAN_izqA caso de que la paridad sea impar
o EAN_izqB caso de que sea par, la paridad se obtiene con el primer dígito del ISBN.Para codificar
los últimos 6 números se codifican con el array EAN_dcha */
char EAN_izqA[10][8] = {"0001101","0011001","0010011","0111101","0100011","0110001","0101111","0111011","0110111","0001011"};
char EAN_izqB[10][8] = {"0100111","0110011","0011011","0100001","0011101","0111001","0000101","0010001","0001001","0010111"};
char EAN_dcha[10][8] = {"1110010","1100110","1101100","1000010","1011100","1001110","1010000","1000100","1001000","1110100"};
char CodificaIzq[10][6] = { "AAAAA","ABABB","ABBAB","ABBBA","BAABB","BBAAB","BBBAA","BABAB","BABBA","BBABA" };
GtkWidget *ISBN,*EAN13;
GdkWindow *ven;
GdkGC* gc;
char str2[95],nISBN[14]; // En str2 almacenaremos el número en EAN13 al completo y en nISBN el número ISBN en una cadena.
unsigned long int n[13]; // Este almacena el número ISBN entero, pero en un array de enteros.

int main(int argc, char *argv[]){

  GtkWidget *window,*vbox,*frame_codigo,*frame_listado,*hboxBot,*frame, *bbox, *vboxA;
  GtkWidget *frame2, *bbox2, *vboxA2;
  GtkWidget *button_ISBN,*button_EAN13;
  GtkWidget *miniframe1, *miniframe2,*area;
  GdkColormap *colormap;
  GdkColor white;

  gtk_init(&argc, &argv);
  
  // Inicializamos la ventana principal que contendrá los box.
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_size_request(window, ancho, alto);
  gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER_ALWAYS); 
  gtk_window_set_title (GTK_WINDOW (window), "Codificador JABA 1.7");

  // Creamos el box principal vertical.
  vbox = gtk_vbox_new(0,5);
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  // Y aquí creamos el frame que contendrá el área donde dibujaremos el código
  // de barras.
  frame_codigo = gtk_frame_new("Código de Barras EAN13");
  gtk_box_pack_start(GTK_BOX(vbox), frame_codigo, 0, 1, 10);
  gtk_frame_set_shadow_type(GTK_FRAME(frame_codigo), GTK_SHADOW_ETCHED_OUT);
  area = gtk_drawing_area_new();
  gtk_drawing_area_size(GTK_DRAWING_AREA(area),780,300);

  colormap = gtk_widget_get_colormap(GTK_WIDGET(window));
  gdk_colormap_alloc_color(colormap, &white, TRUE, TRUE);
  gdk_color_parse("white", &white);
  gtk_widget_modify_bg(area, GTK_STATE_NORMAL, &white);
  gtk_container_add(GTK_CONTAINER(frame_codigo),area);

  // Aquí creamos el frame que contendrá los 2 entrys donde pondremos
  // el isbn o el ean13, lo que queramos calcular.
  frame_listado = gtk_frame_new("Datos");
  gtk_box_pack_start(GTK_BOX(vbox), frame_listado, 1, 1, 10);
  gtk_frame_set_shadow_type(GTK_FRAME(frame_listado), GTK_SHADOW_ETCHED_OUT);


  vboxA = gtk_vbox_new(0,0);
  frame = gtk_frame_new("");
  miniframe1 = gtk_frame_new("ISBN");
  miniframe2 = gtk_frame_new("EAN");

  ISBN = gtk_entry_new_with_max_length(13);
  EAN13 = gtk_entry_new_with_max_length(84);
  gtk_container_add(GTK_CONTAINER(miniframe1),ISBN);
  gtk_container_add(GTK_CONTAINER(miniframe2),EAN13);
  gtk_box_pack_start(GTK_BOX(vboxA), miniframe1, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vboxA), miniframe2, FALSE, FALSE, 0);

  bbox = gtk_hbutton_box_new();
  gtk_container_set_border_width(GTK_CONTAINER(bbox), 5);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_START);
  gtk_button_box_set_spacing(GTK_BUTTON_BOX(bbox), 20);
  button_ISBN = gtk_button_new_with_label("Calcular ISBN");
  button_EAN13 = gtk_button_new_with_label("Calcular EAN13");
  gtk_container_add(GTK_CONTAINER(bbox), button_ISBN);
  gtk_container_add(GTK_CONTAINER(bbox), button_EAN13);
  gtk_container_add(GTK_CONTAINER(vboxA),bbox);
  gtk_container_add(GTK_CONTAINER(frame),vboxA);

  // Este frame simplemente está para que el anterior ocupé la mitad de la pantalla.
  frame2 = gtk_frame_new("");
  bbox2 = gtk_hbutton_box_new();
  gtk_container_set_border_width(GTK_CONTAINER(bbox2), 5);
  gtk_container_add(GTK_CONTAINER(frame2), bbox2);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox2), GTK_BUTTONBOX_START);
  gtk_button_box_set_spacing(GTK_BUTTON_BOX(bbox2), 20);
  gtk_button_box_set_child_size(GTK_BUTTON_BOX(bbox2), 10, 0);


  hboxBot = gtk_hbox_new(0, 0);
  gtk_container_set_border_width(GTK_CONTAINER(hboxBot), 10);
  gtk_container_add(GTK_CONTAINER(frame_listado), hboxBot);
  gtk_container_add(GTK_CONTAINER(hboxBot), frame);
  gtk_container_add(GTK_CONTAINER(hboxBot), frame2);

  gtk_signal_connect(GTK_OBJECT(area),"expose_event",GTK_SIGNAL_FUNC(DibujarArea),NULL);
  gtk_signal_connect_object(GTK_OBJECT(button_ISBN), "clicked", GTK_SIGNAL_FUNC(calculaISBN), EAN13);
  gtk_signal_connect_object(GTK_OBJECT(button_EAN13), "clicked", GTK_SIGNAL_FUNC(calculaEAN13), ISBN);
  gtk_signal_connect(GTK_OBJECT(button_EAN13), "clicked", GTK_SIGNAL_FUNC(DibujarCodBarras), area);
  gtk_signal_connect(GTK_OBJECT(button_ISBN), "clicked", GTK_SIGNAL_FUNC(DibujarCodBarras), area); 
  gtk_signal_connect(GTK_OBJECT(ISBN), "activate", GTK_SIGNAL_FUNC(calculaEAN13), ISBN);
  gtk_signal_connect(GTK_OBJECT(EAN13), "activate", GTK_SIGNAL_FUNC(calculaISBN), EAN13);
  gtk_signal_connect(GTK_OBJECT(EAN13), "activate", GTK_SIGNAL_FUNC(DibujarCodBarras), area);
  gtk_signal_connect(GTK_OBJECT(ISBN), "activate", GTK_SIGNAL_FUNC(DibujarCodBarras), area); 
  g_signal_connect (G_OBJECT (window), "delete-event", gtk_main_quit, NULL);
  gtk_widget_show_all (window);

  gtk_main();
  return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
void CodigoErroneo(){
  GtkWidget *dialogo,*window3;			
  window3 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window3), "");
  gtk_signal_connect(GTK_OBJECT(window3), "destroy", GTK_SIGNAL_FUNC(gtk_widget_destroy), NULL);
  gtk_widget_set_usize(window3,10,10);
  dialogo = gtk_message_dialog_new(GTK_WINDOW(window3),GTK_DIALOG_MODAL || GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"El código introducido es incorrecto");
  gtk_dialog_run(GTK_DIALOG(dialogo));
  gtk_widget_destroy(dialogo);
  gtk_widget_destroy(window3);
	
}
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
void DibujarArea(GtkWidget *widget){
   gc = gdk_gc_new(widget->window);
   ven = widget->window;
   
   gdk_draw_rectangle(ven,gc,0,10,10,770,280);  
}
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
int VerificaISBN(){
  int i,suma=0;
  for(i=0;i<13;i++){
	if (i%2==0) suma = suma + n[i];
	else suma = suma + n[i]*3;
  }
  //printf("%u\n",suma);
  if(suma%10 == 0) return 1;
  else return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
void DibujarCodBarras(GtkWidget *widget){

   int i=1,j,z; 
   char mosISBN[2];
   if (VerificaISBN()==1){
   GdkFont *font; 
   font=gdk_font_load("-adobe-helvetica-medium-r-normal--*-120-*-*-*-*-*-*"); 

   mosISBN[0] = nISBN[0];
   mosISBN[1] = '\0';
   gdk_draw_string(ven,font,gc,INICIO-12,CORDYINICIAL,mosISBN);
   // Pintamos el 101 del principio
   gdk_draw_line(ven,gc,INICIO,CORDYINICIAL,INICIO,CORDYFINAL);  
   gdk_draw_line(ven,gc,INICIO+1,CORDYINICIAL,INICIO+1,CORDYFINAL); 
   gdk_draw_line(ven,gc,INICIO+4,CORDYINICIAL,INICIO+4,CORDYFINAL);  
   gdk_draw_line(ven,gc,INICIO+5,CORDYINICIAL,INICIO+5,CORDYFINAL); 
   // Pintamos el 01010 del medio
   gdk_draw_line(ven,gc,CENTRO,CORDYINICIAL,CENTRO,CORDYFINAL);  
   gdk_draw_line(ven,gc,CENTRO+1,CORDYINICIAL,CENTRO+1,CORDYFINAL);
   gdk_draw_line(ven,gc,CENTRO+4,CORDYINICIAL,CENTRO+4,CORDYFINAL);  
   gdk_draw_line(ven,gc,CENTRO+5,CORDYINICIAL,CENTRO+5,CORDYFINAL);    
   // Pintamos el 101 del final
   gdk_draw_line(ven,gc,FIN,CORDYINICIAL,FIN,CORDYFINAL);  
   gdk_draw_line(ven,gc,FIN+1,CORDYINICIAL,FIN+1,CORDYFINAL); 
   gdk_draw_line(ven,gc,FIN+4,CORDYINICIAL,FIN+4,CORDYFINAL);  
   gdk_draw_line(ven,gc,FIN+5,CORDYINICIAL,FIN+5,CORDYFINAL); 
   // Con este primer for, pintamos la parte izquierda
   for(j=0;j<42;j++){
        z = j*2;
	if (str2[j]=='1') {
   			gdk_draw_line(ven,gc,INICIO+z+6,CORDYINICIAL-15,INICIO+z+6,CORDYFINAL);  
   			gdk_draw_line(ven,gc,INICIO+z+7,CORDYINICIAL-15,INICIO+z+7,CORDYFINAL); 
			}
	mosISBN[0] = nISBN[j/7+1];
	mosISBN[1] = '\0';
        if (j%7==0) gdk_draw_string(ven,font,gc,INICIO+z+8,CORDYINICIAL,mosISBN);
    }
   // Con este for, pintamos la parte derecha
   for(j=42;j<85;j++){
        z = (j-42)*2;
	if (str2[j]=='1') {
   			gdk_draw_line(ven,gc,CENTRO+z+8,CORDYINICIAL-15,CENTRO+z+8,CORDYFINAL);  
   			gdk_draw_line(ven,gc,CENTRO+z+9,CORDYINICIAL-15,CENTRO+z+9,CORDYFINAL); 
			}
	mosISBN[0] = nISBN[j/7+1]; // Guarda el numero de ISBN del binario al que corresponde
	mosISBN[1] = '\0'; // Para mostrarlo en pantalla mediante una cadena.
        if (j%7==0) gdk_draw_string(ven,font,gc,CENTRO+z+8,CORDYINICIAL,mosISBN);
    }
  }
  else CodigoErroneo();
}
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
void calculaEAN13(GtkWidget *wid){
  const gchar *str;
  int i;
  str = gtk_entry_get_text(GTK_ENTRY(wid));
  nISBN[0] = '\0';
  strcat(nISBN,str);
  str2[0] = '\0'; 
  for(i=0; i<13;i++){
  	n[i] = (str[i] - '0');
   }
  if (VerificaISBN()==1) {
  strcat(str2,EAN_izqA[n[1]]);
  // Con este for pasamos de ISBN a EAN13 de la siguiente forma:
  // + En la parte izquierda si, si es de paridad A, lo busca en
  //   EAN_izqA, en caso de que sea de paridad B, buscará en 
  //   EAN_izqB.
  // + Si estamos en la parte derecha, directamente busca en EAN_dcha.
  for(i=2;i<13;i++){
	if (i<7) {
 		if(CodificaIzq[n[0]][i-2] == 'A') {
				strcat(str2,EAN_izqA[n[i]]);
				}
		else {
			strcat(str2,EAN_izqB[n[i]]);
		}

	}	
	else strcat(str2,EAN_dcha[n[i]]); 
  }

  gtk_entry_set_text(GTK_ENTRY(EAN13),str2);
  }
  
}
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
void calculaISBN(GtkWidget *wid){
  const gchar *str;
  char codIzq[6],r2[130],str3[7],r1[200],rigor[40],strL2[8],aux;
  int i,j=0,k,l,paridad=0,num=12;

  str = gtk_entry_get_text(GTK_ENTRY(wid));
  nISBN[0] = '\0';

  for(i=0;i<7;i++){
              	  str3[i] = str[i];
		}
  str3[7] = '\0';
  // Este while nos comprueba si el usuario ha insertado un código de barras que se lee de izquierda
  // a derecha caso de ser de paridad IMPAR (A) , o de dcha a izquierda en caso de ser paridad PAR (B).
  while((j<10)&&(paridad<7)) {   
			paridad = 0;
			while ((str3[paridad]==EAN_izqA[j][paridad])&&(str3[paridad]!='\0')&&(EAN_izqA[j][paridad]!='\0')) paridad++;
                        j++;
		}
  i=0;
  str2[0] = '\0';

  // Para leer de izquierda a derecha, entra en este bucle.
  while ((i<85)&&(paridad==7)){
	if(i%7 == 0) {
			for (j=0;j<10;j++){ 
					k = 0;
					if(i<43){
						while ((strL2[k]==EAN_izqA[j][k])&&(strL2[k]!='\0')&&(EAN_izqA[j][k]!='\0')) k++;
						if (k==7){
							  	if(i!=7) strcat(codIzq,"A");	
							  	n[(i/7)] = j;
								 }
						else{
							k = 0;
							while ((strL2[k]==EAN_izqB[j][k])&&(strL2[k]!='\0')&&(EAN_izqB[j][k]!='\0')) k++;
							if (k==7){
							  	if(i!=7) strcat(codIzq,"B");	
							  	n[(i/7)] = j;
								 }
						    }
						}
						
					else {   k = 0;
						 while ((strL2[k]==EAN_dcha[j][k])&&(strL2[k]!='\0')&&(EAN_dcha[j][k]!='\0')) k++;
						 if (k==7) n[(i/7)] = j;			
					     } 
						
					}
			strL2[0] = '\0'; 
		  	}	
		aux = str[i];
		strcat(strL2,&aux);
		strcat(str2,&aux);
	i++;
  } 
 i=0;
 if(paridad!=7) strL2[0] = '\0';
  // Para leer de derecha a izquierda, entra en este bucle.
 while ((i<85)&&(paridad!=7)){
	if(i%7 == 0) {
			for (j=0;j<10;j++){ 
					k = 0;
					l = 6;
					if(num<7){
						while ((strL2[k]==EAN_izqA[j][l])&&(strL2[k]!='\0')){ k++; l--; }
						if (k==7){
							  if(i!=77) strcat(codIzq,"A");	
							  n[num] = j;
							  num--;
							}
						else{
							k = 0;
							l = 6;
							while ((strL2[k]==EAN_izqB[j][l])&&(strL2[k]!='\0')){ k++; l--; }
							if (k==7){
							  	if(i!=77) strcat(codIzq,"B");	
							  	n[num] = j;
								num--;
								 }
						    }

						}
						
					else {   k = 0;
						 l = 6;
						 while ((strL2[k]==EAN_dcha[j][l])&&(strL2[k]!='\0')) { k++; l--; }
						 if (k==7) {
							    n[num] = j;	
							    num--;		
							   }
					     } 
						
					}
			strL2[0] = '\0'; 
		  	}	
		aux = str[i];
		strcat(strL2,&aux);
		strcat(str2,&aux);
 
	i++;
  } 
  // Con este for, calculamos el primer dígito.
  if (VerificaISBN()==1){
  for(j=0;j<10;j++){k=0;
		 while ((codIzq[k]==CodificaIzq[j][k])&&(codIzq[k]!='\0')&&(CodificaIzq[j][k]!='\0')) k++;
		 if(k==5) n[0] = j;
		}
  // Mediante este bucle, pasamos a tener nuestro ISBN en una cadena.
  for(i=0;i<13;i++) nISBN[i] = '0' + n[i];
  nISBN[13] = '\0';
  gtk_entry_set_text(GTK_ENTRY(ISBN),nISBN);
  }
}
