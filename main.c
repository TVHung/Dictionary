#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <string.h>
#include "./bt-5.0.0/inc/btree.h"

GtkWidget *meanTxt;
GtkWidget *historyTxt; 
GtkWidget *addMeanTxt;
GtkWidget *repairTxt;
GtkWidget *searchEntry;
GtkWidget *addEntry,*updateEntry;
GtkWidget *window,*window_add_word,*window_edit_word, *window_infor, *window_help;
GtkListStore *list;
GtkTreeIter Iter;

BTA *book;

char historytext[5000];
char suggetword[40];
int flag,sai=1;

void open_book(){
    book = btopn("english_vietnamese.dat",0,0);
}

GdkPixbuf *create_pixbuf(const gchar * filename){
    GdkPixbuf *pixbuf;
    GError *error = NULL;
    pixbuf = gdk_pixbuf_new_from_file(filename, &error);
    if (!pixbuf) {
      fprintf(stderr, "%s\n", error->message);
      g_error_free(error);
    }
    return pixbuf;
}

void findwordmean(char *word){
    char mean[5000];
    int size;
    if(btsel(book,word,mean,5000,&size)){//xác định dữ liệu của khóa có sẵn, trả lại mean record của key tồn tại.
        set_textview_text("\n  This word does not exist.\n   Please try again or press the 'Add' key to add new words.\n");
    }
    else{
        set_textview_text(mean);
        strcat(historytext,word);//noi tiep lich su
        strcat(historytext,"\n");
        set_history_textview_text(historytext);
    }
}

void clicked_search(){
    char word[40];
    strcpy(word,gtk_entry_get_text(GTK_ENTRY(searchEntry)));
    findwordmean(word);   
}


void add_word(){// xu cua so 
    GtkBuilder *builder;
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "window_main.glade", NULL);
    window_add_word = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
    gtk_builder_connect_signals(builder, NULL);
    addEntry = GTK_WIDGET(gtk_builder_get_object(builder, "addEntry"));
    addMeanTxt = GTK_WIDGET(gtk_builder_get_object(builder, "addMeanTxt"));
    g_object_unref(builder);
    gtk_window_set_position(GTK_WINDOW(window_add_word), GTK_WIN_POS_CENTER);
    gtk_widget_show(window_add_word);
}

void click_add_word(){// xu ly tu
    GtkTextIter st_iter;
    GtkTextIter ed_iter;
    char word[40];
    strcpy(word,gtk_entry_get_text(GTK_ENTRY(addEntry)));
    gtk_text_buffer_get_start_iter (gtk_text_view_get_buffer(GTK_TEXT_VIEW(addMeanTxt)), &st_iter);
    gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer(GTK_TEXT_VIEW(addMeanTxt)), &ed_iter);
    char *mean =  gtk_text_buffer_get_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(addMeanTxt)), &st_iter, &ed_iter, FALSE);
    if(btins(book,word,mean,strlen(mean)+1) || strlen(word) == 0) {//Chèn WORD và data vào B tree
        Show_message(window_add_word, GTK_MESSAGE_ERROR, "Error!", "Add failed, the word may already exist");
    }else 
        Show_message(window_add_word, GTK_MESSAGE_INFO, "Success!", "Added from success.");
    gtk_widget_destroy(window_add_word);
}

void repair_word(){
    GtkBuilder *builder;
    char word[40];
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "window_main.glade", NULL);
    window_edit_word = GTK_WIDGET(gtk_builder_get_object(builder, "window2"));
    gtk_builder_connect_signals(builder, NULL);

    updateEntry = GTK_WIDGET(gtk_builder_get_object(builder, "updateEntry"));
    repairTxt = GTK_WIDGET(gtk_builder_get_object(builder, "repairTxt"));

    strcpy(word,gtk_entry_get_text(GTK_ENTRY(searchEntry)));// lay du lieu de add vao repairTxt
    gtk_entry_set_text(GTK_ENTRY(updateEntry),word);
    g_object_unref(builder);
    gtk_window_set_position(GTK_WINDOW(window_edit_word), GTK_WIN_POS_CENTER);
    gtk_widget_show(window_edit_word); 
}

void click_update_word(){
    GtkTextIter st_iter;
    GtkTextIter ed_iter;
    char word[40];
    strcpy(word,gtk_entry_get_text(GTK_ENTRY(searchEntry)));
    gtk_text_buffer_get_start_iter (gtk_text_view_get_buffer(GTK_TEXT_VIEW(repairTxt)), &st_iter);
    gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer(GTK_TEXT_VIEW(repairTxt)), &ed_iter);
    char * mean =  gtk_text_buffer_get_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(repairTxt)), &st_iter, &ed_iter, FALSE);
    if(strlen(word) != 0){
        btupd(book,word,mean,strlen(mean)+1);//update data cho key
        Show_message(window_edit_word, GTK_MESSAGE_INFO, "Success!", "Edit the word successfully");
    }else
        Show_message(window_edit_word, GTK_MESSAGE_INFO, "Error!", "Edit from failed");
    gtk_widget_destroy(window_edit_word);
}

void press_xoa_tu(){
    char word[40];
    strcpy(word,gtk_entry_get_text(GTK_ENTRY(searchEntry)));
    if(strlen(word)){
        btdel(book, word);// xoa tu khoi btree
        Show_message(window, GTK_MESSAGE_ERROR, "Deleted", "Delete the word successfully.");
    }else
        Show_message(window, GTK_MESSAGE_ERROR, "Error", "Delete word failed.");
}

void btn_infor(){
    GtkBuilder *builder;
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "window_main.glade", NULL);
    window_infor = GTK_WIDGET(gtk_builder_get_object(builder, "window_infor"));
    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);
    gtk_window_set_position(GTK_WINDOW(window_infor), GTK_WIN_POS_CENTER);
    gtk_widget_show(window_infor); 
}

void click_help_btn(){
    GtkBuilder *builder;
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "window_main.glade", NULL);
    window_help = GTK_WIDGET(gtk_builder_get_object(builder, "window_help"));
    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);
    gtk_window_set_position(GTK_WINDOW(window_help), GTK_WIN_POS_CENTER);
    gtk_widget_show(window_help); 
}

void btn_close_help(){
    gtk_widget_destroy(window_help);
}

void btn_close_infor(){
    gtk_widget_destroy(window_infor);
}

void btn_quit(){
    gtk_widget_destroy(window);
}

void cancel_add(){
    gtk_widget_destroy(window_add_word);
}

void cancel_repair(){
    gtk_widget_destroy(window_edit_word);
}

void set_textview_text(char * text) {
    GtkTextBuffer *buffer;
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(meanTxt));
    if (buffer == NULL) {
        printf("Get buffer fail!");
        buffer = gtk_text_buffer_new(NULL);
    }
    gtk_text_buffer_set_text(buffer, text, -1);
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(meanTxt), buffer);
}

void set_history_textview_text(char * text) {
    GtkTextBuffer *buffer;
    GtkTextIter st_iter;
    GtkTextIter ed_iter;  
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(historyTxt));
    gtk_text_buffer_set_text(buffer, text, -1);
    gtk_text_buffer_get_start_iter (gtk_text_view_get_buffer(GTK_TEXT_VIEW(historyTxt)), &st_iter);
    gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer(GTK_TEXT_VIEW(historyTxt)), &ed_iter);
    char *histext =  gtk_text_buffer_get_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(historyTxt)), &st_iter, &ed_iter, FALSE);
    gtk_text_buffer_set_text(buffer,histext, -1);
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(historyTxt), buffer);
}

void on_key_press (GtkWidget *widget, GdkEventKey *event, gpointer user_data){
    char word[40];
    int x;
    strcpy(word, gtk_entry_get_text(GTK_ENTRY(searchEntry)));
    x = strlen(word); 
    if(x == 0) sai=1;
    if(event->keyval == GDK_KEY_F1)
        add_word();
    if(event->keyval == GDK_KEY_F2)
        repair_word();
    if(event->keyval == GDK_KEY_F3)
        press_xoa_tu();
    if(event->keyval == GDK_KEY_F4)
        click_help_btn();
    if(event->keyval == GDK_KEY_F5)
        btn_quit();
    if(event->keyval == GDK_KEY_F6)
        btn_infor();
    if (event->keyval != GDK_KEY_BackSpace){
        if(event->keyval == GDK_KEY_Tab){
            gtk_entry_set_text(GTK_ENTRY(searchEntry),suggetword);
            findwordmean(suggetword);
        }
        else{
            word[x]=event->keyval;
            word[x+1]='\0';
            flag=0;
            gtk_list_store_clear(list);
            if(sai!=0)
                kiem_tra_tu(word);
        }   
    }
    else {
        if(x!=1){
            word[x - 1] = '\0';
            if(word[0]!= '\0')
            flag=0;
            sai=1;
            gtk_list_store_clear(list);
            if(strlen(word) <= strlen(suggetword))
            kiem_tra_tu(word);  
        } 
    }
}

void enter_search(){
    char word[40];
    strcpy(word, gtk_entry_get_text(GTK_ENTRY(searchEntry)));
    gtk_entry_set_text(GTK_ENTRY(searchEntry),word);
    findwordmean(word);
}

void kiem_tra_tu(char *word1){//tao danh sach goi y
    int dai1, size, i, j, first = 0, k = 0, q = 0;
    char listword[40], word[40], mean[5000];
    char kd[1];
    kd[0] = word1[0];
    kd[1] = '\0';
    dai1  = strlen(word1);
    flag = 1;
    btsel(book,kd,mean,sizeof(char*),&size);//xac dinh du lieu co san
    while(btseln(book,word,mean,5000,&size)==0 ){
        i = 0;
        j = 0;
        if(flag == 0) break;
        while(1){
            if(flag == 0) break;
            if(word1[0] < word[0])    q = 1;   
            if(word1[i] == word[i])   i++;
            else
                break;
            if(i == dai1){
                j=1;
                if(first == 0) 
                    strcpy(suggetword,word);
                first++;
                break;
            }
        }
        if(flag == 0 || q == 1) break;
        sai = 0;
        if(j == 1){
            sai = 1;
            if(flag == 0) break;
            strcpy(listword, word);
            gtk_list_store_append(list, &Iter);//them list moi
            gtk_list_store_set(list, &Iter, 0, listword, -1 );
            k++;
            if(flag == 0 || k == 15) break;
        }
    }
}

void Show_message(GtkWidget * parent , GtkMessageType type,  char * mms, char * content){
    GtkWidget *mdialog;
    mdialog = gtk_message_dialog_new(GTK_WINDOW(parent), GTK_DIALOG_DESTROY_WITH_PARENT, type,GTK_BUTTONS_OK, "%s", mms);
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(mdialog), "%s",  content);
    gtk_dialog_run(GTK_DIALOG(mdialog));
    gtk_widget_destroy(mdialog);
}


int main(int argc, char const *argv[]){
    GtkBuilder *builder;
    GtkEntryCompletion *comple;
    GdkPixbuf *icon; 

    gtk_init(&argc, &argv);
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "window_main.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    gtk_builder_connect_signals(builder, NULL);//ket noi giua giao dien vs builder
    
    meanTxt     = GTK_WIDGET(gtk_builder_get_object(builder, "meanTxt"));
    historyTxt  = GTK_WIDGET(gtk_builder_get_object(builder, "historyTxt"));
    searchEntry = GTK_WIDGET(gtk_builder_get_object(builder, "searchentry1"));

    comple = gtk_entry_completion_new();
    gtk_entry_completion_set_text_column(comple, 0);
    list   = gtk_list_store_new(1, G_TYPE_STRING);
    gtk_entry_completion_set_model(comple, GTK_TREE_MODEL(list));
    gtk_entry_set_completion(GTK_ENTRY(searchEntry), comple);

    g_signal_connect (searchEntry, "key_press_event", G_CALLBACK (on_key_press), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK (gtk_main_quit), NULL);// khi tat chuong trinh thi terminal cung tat

    icon = create_pixbuf("ev.png");  
    gtk_window_set_icon(GTK_WINDOW(window), icon);
    if(!btinit())
        open_book();
    g_object_unref(builder);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_widget_show(window);                
    gtk_main();

    btcls(book);
    return 0;
}
