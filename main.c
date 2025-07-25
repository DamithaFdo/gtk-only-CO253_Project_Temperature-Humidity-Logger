#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include "simulator.h"
#include "stats.h"

#define LOG_SIZE 50
#define ALERT_THRESHOLD 65

Reading readings[LOG_SIZE];
Statistics stats;

static GtkListStore *list_store;
static GtkWidget *alert_label;

void update_gui(int idx) {
    GtkTreeIter iter;
    gtk_list_store_append(list_store, &iter);

    char safe_temp[32], safe_humid[32];
    if (readings[idx].temperature > ALERT_THRESHOLD)
        sprintf(safe_temp, "THRESHOLD TEMP EXCEEDED");
    else
        sprintf(safe_temp, "TEMP in safe limits");

    if (readings[idx].humidity > ALERT_THRESHOLD)
        sprintf(safe_humid, "THRESHOLD HUMID EXCEEDED");
    else
        sprintf(safe_humid, "HUMID in safe limits");

    gtk_list_store_set(list_store, &iter,
        0, readings[idx].seq_no,
        1, readings[idx].timestamp,
        2, readings[idx].temperature,
        3, readings[idx].humidity,
        4, safe_temp,
        5, safe_humid,
        -1);

    char alert_msg[128];
    sprintf(alert_msg, "%s | %s", safe_temp, safe_humid);
    gtk_label_set_text(GTK_LABEL(alert_label), alert_msg);
}

gboolean simulate_reading(gpointer data) {
    static int idx = 0;
    if (idx >= LOG_SIZE) {
        calc_statistics(readings, LOG_SIZE, &stats);
        // Show statistics dialog
        char stats_msg[256];
        sprintf(stats_msg,
            "Average Temp: %.2f\nAverage Humid: %.2f\n"
            "Max Temp: %.2f at %s\n"
            "Max Humid: %.2f at %s\n",
            stats.avg_temp,
            stats.avg_humid,
            stats.max_temp, stats.max_temp_ts,
            stats.max_humid, stats.max_humid_ts
        );
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", stats_msg);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return FALSE;
    }
    update_gui(idx);
    idx++;
    return TRUE;
}

void start_simulation(GtkWidget *widget, gpointer data) {
    int input_mode = *(int*)data;
    if (input_mode == 1) {
        generate_random_readings(readings, LOG_SIZE);
    } else {
        if (!read_csv_readings("readings.csv", readings, LOG_SIZE)) {
            GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
                GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "CSV file error!");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
            return;
        }
    }
    gtk_widget_set_sensitive(widget, FALSE);
    g_timeout_add_seconds(2, simulate_reading, NULL); // 2 seconds sleep
}

static void on_dropdown_changed(GtkComboBox *combo, gpointer mode_ptr) {
    *(int*)mode_ptr = gtk_combo_box_get_active(combo) + 1;
}

void build_gui(int *input_mode) {
    GtkWidget *window, *vbox, *tree, *button, *dropdown;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *col;

    gtk_init(NULL, NULL);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Temp/Humidity Logger");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Dropdown for input selection
    dropdown = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(dropdown), "Random Generator");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(dropdown), "CSV File Input");
    gtk_combo_box_set_active(GTK_COMBO_BOX(dropdown), 0);
    gtk_box_pack_start(GTK_BOX(vbox), dropdown, FALSE, FALSE, 0);

    button = gtk_button_new_with_label("Start Simulation");
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);

    list_store = gtk_list_store_new(6, G_TYPE_INT, G_TYPE_STRING, G_TYPE_DOUBLE, G_TYPE_DOUBLE, G_TYPE_STRING, G_TYPE_STRING);
    tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_store));
    const char *titles[] = {"Seq No", "Timestamp", "Temp", "Humid", "Temp Status", "Humid Status"};
    for (int i = 0; i < 6; i++) {
        renderer = gtk_cell_renderer_text_new();
        col = gtk_tree_view_column_new_with_attributes(titles[i], renderer, "text", i, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree), col);
    }
    gtk_box_pack_start(GTK_BOX(vbox), tree, TRUE, TRUE, 0);

    alert_label = gtk_label_new("Waiting for simulation...");
    gtk_box_pack_start(GTK_BOX(vbox), alert_label, FALSE, FALSE, 0);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    g_signal_connect(button, "clicked", G_CALLBACK(start_simulation), input_mode);

    g_signal_connect(dropdown, "changed", G_CALLBACK(on_dropdown_changed), input_mode);

    gtk_widget_show_all(window);
    gtk_main();
}

int main(int argc, char *argv[]) {
    int input_mode = 1; // 1: random, 2: CSV
    build_gui(&input_mode);
    return 0;
}