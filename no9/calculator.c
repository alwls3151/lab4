#include <gtk/gtk.h>
#include <string.h>

GtkWidget *entry;
char current_text[256] = "";

// 버튼 클릭 이벤트 핸들러
void on_button_clicked(GtkWidget *widget, gpointer data) {
    const char *label = gtk_button_get_label(GTK_BUTTON(widget));

    if (strcmp(label, "C") == 0) {
        current_text[0] = '\0';
    } else if (strcmp(label, "=") == 0) {
        double result = 0;
        char operator;
        double num1, num2;

        if (sscanf(current_text, "%lf %c %lf", &num1, &operator, &num2) == 3) {
            switch (operator) {
                case '+': result = num1 + num2; break;
                case '-': result = num1 - num2; break;
                case '*': result = num1 * num2; break;
                case '/': result = (num2 != 0) ? num1 / num2 : 0; break;
                default: strcpy(current_text, "Error"); break;
            }
            snprintf(current_text, sizeof(current_text), "%g", result);
        } else {
            strcpy(current_text, "Error");
        }
    } else {
        strcat(current_text, label);
    }

    gtk_entry_set_text(GTK_ENTRY(entry), current_text);
}

// GTK+ 애플리케이션 초기화
void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *grid;

    // 창 생성
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 250, 300);

    // 그리드 위젯 생성
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // 입력창 생성
    entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 0, 4, 1);

    // 버튼 레이블
    const char *buttons[] = {
        "7", "8", "9", "/",
        "4", "5", "6", "*",
        "1", "2", "3", "-",
        "C", "0", "=", "+"
    };

    // 버튼 추가
    int pos = 0;
    for (int row = 1; row <= 4; row++) {
        for (int col = 0; col < 4; col++) {
            GtkWidget *button = gtk_button_new_with_label(buttons[pos]);
            g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);
            gtk_grid_attach(GTK_GRID(grid), button, col, row, 1, 1);
            pos++;
        }
    }

    // 창 표시
    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.example.calculator", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
