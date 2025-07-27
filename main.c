#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simulator.h"
#include "stats.h"

//x86_64-w64-mingw32-gcc main.c simulator.c stats.c test.c -o app $(pkg-config --cflags --libs gtk+-3.0)

// Constants
#define MAX_LOG_SIZE 50        // Maximum # readings
#define ALERT_THRESHOLD 65      // Exceeded threshold
#define WARNING_THRESHOLD 60    // Warning threshold

// Global data structure
Reading readings[MAX_LOG_SIZE];
Statistics stats;

static GtkListStore *list_store;
static GtkWidget *tree_view;
static GtkWidget *readings_spinner;
static GtkTreeIter current_iter;
static gboolean has_current_iter = FALSE;

// Global variables to manage application state and user preferences
static int selected_log_size = 50;                  // Default # of readings
static int input_mode = 1;                          // Input modes 1=random, 2=default CSV, 3=test CSV
static char csv_filename[256] = "readings.csv";     // Simulation data csv file

// Humidity Status col
void temp_status_cell_data_func(GtkTreeViewColumn *col, GtkCellRenderer *renderer,
                               GtkTreeModel *model, GtkTreeIter *iter, gpointer data) {
    gchar *status;
    gtk_tree_model_get(model, iter, 4, &status, -1);
    
    const char *color;
    if (strstr(status, "Alert!") != NULL) {
        color = "red";
    } else if (strstr(status, "Warning!") != NULL) {
        color = "orange";
    } else {
        color = "green";
    }
    
    // Make the current row selected
    gboolean is_current = FALSE;
    if (has_current_iter) {
        GtkTreePath *current_path = gtk_tree_model_get_path(model, &current_iter);
        GtkTreePath *iter_path = gtk_tree_model_get_path(model, iter);
        if (gtk_tree_path_compare(current_path, iter_path) == 0) {
            is_current = TRUE;
        }
        gtk_tree_path_free(current_path);
        gtk_tree_path_free(iter_path);
    }
    
    g_object_set(renderer,
        "foreground", color,
        "weight", PANGO_WEIGHT_BOLD,
        "background", is_current ? "white" : NULL,
        NULL);
    g_free(status);
}

// Humidity Status col
void humid_status_cell_data_func(GtkTreeViewColumn *col, GtkCellRenderer *renderer,
                               GtkTreeModel *model, GtkTreeIter *iter, gpointer data) {
    gchar *status;
    gtk_tree_model_get(model, iter, 5, &status, -1);
    
    const char *color;
    if (strstr(status, "Alert!") != NULL) {
        color = "red";
    } else if (strstr(status, "Warning!") != NULL) {
        color = "orange";
    } else {
        color = "green";
    }
    
    gboolean is_current = FALSE;
    if (has_current_iter) {
        GtkTreePath *current_path = gtk_tree_model_get_path(model, &current_iter);
        GtkTreePath *iter_path = gtk_tree_model_get_path(model, iter);
        if (gtk_tree_path_compare(current_path, iter_path) == 0) {
            is_current = TRUE;
        }
        gtk_tree_path_free(current_path);
        gtk_tree_path_free(iter_path);
    }
    
    g_object_set(renderer,
        "foreground", color,
        "weight", PANGO_WEIGHT_BOLD,
        "background", is_current ? "white" : NULL,
        NULL);
    g_free(status);
}

// Other columns
void generic_cell_data_func(GtkTreeViewColumn *col, GtkCellRenderer *renderer,
                           GtkTreeModel *model, GtkTreeIter *iter, gpointer data) {

    gboolean is_current = FALSE;
    if (has_current_iter) {
        GtkTreePath *current_path = gtk_tree_model_get_path(model, &current_iter);
        GtkTreePath *iter_path = gtk_tree_model_get_path(model, iter);
        if (gtk_tree_path_compare(current_path, iter_path) == 0) {
            is_current = TRUE;
        }
        gtk_tree_path_free(current_path);
        gtk_tree_path_free(iter_path);
    }
    
    g_object_set(renderer,
        "background", is_current ? "white" : NULL,
        "weight", is_current ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL,
        NULL);
}

void update_gui(int idx) {
    GtkTreeIter iter;
    gtk_list_store_append(list_store, &iter);

    // Value formatting
    char temp_str[32], humid_str[32], safe_temp[50], safe_humid[50];
    sprintf(temp_str, "%.2f °C", readings[idx].temperature);
    sprintf(humid_str, "%.2f %%", readings[idx].humidity);

    /// This is the Main loop determines temperature status///
    if (readings[idx].temperature > ALERT_THRESHOLD)
        strcpy(safe_temp, "Alert! Safe Temperature exceeded.");
    else if (readings[idx].temperature >= WARNING_THRESHOLD)
        strcpy(safe_temp, "Warning! Safe Temperature exceeding.");
    else
        strcpy(safe_temp, "Safe Temperature levels.");

    /// This is the Main loop determines humidity status///
    if (readings[idx].humidity > ALERT_THRESHOLD)
        strcpy(safe_humid, "Alert! Safe Humidity exceeded.");
    else if (readings[idx].humidity >= WARNING_THRESHOLD)
        strcpy(safe_humid, "Warning! Safe Humidity exceeding.");
    else
        strcpy(safe_humid, "Safe Humidity levels.");

    gtk_list_store_set(list_store, &iter,
        0, readings[idx].seq_no,
        1, readings[idx].timestamp,
        2, temp_str,
        3, humid_str,
        4, safe_temp,
        5, safe_humid,
        -1);

    current_iter = iter;
    has_current_iter = TRUE;

    GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(list_store), &iter);
    gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(tree_view), path, NULL, FALSE, 0.0, 0.0);
    
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
    gtk_tree_selection_select_iter(selection, &iter);
    
    gtk_tree_path_free(path);

    gtk_widget_queue_draw(tree_view);
}

gboolean simulate_reading(gpointer data) {
    static int idx = 0;
    if (idx >= selected_log_size) {
        calc_statistics(readings, selected_log_size, &stats);
        
        char stats_msg[512];
        sprintf(stats_msg,
            "Simulation Complete!\n\n"
            "Total Readings: %d\n"
            "Average Temp: %.2f °C\n"
            "Average Humid: %.2f %%\n"
            "Max Temp: %.2f °C at %s\n"
            "Max Humid: %.2f %% at %s\n",
            selected_log_size,
            stats.avg_temp,
            stats.avg_humid,
            stats.max_temp, stats.max_temp_ts,
            stats.max_humid, stats.max_humid_ts
        );
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", stats_msg);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        
        has_current_iter = FALSE;
        gtk_widget_queue_draw(tree_view);
        
        GtkWidget *start_button = (GtkWidget*)data;
        if (start_button) {
            gtk_widget_set_sensitive(start_button, TRUE);
        }
        
        idx = 0;
        return FALSE;
    }
    
    update_gui(idx);
    idx++;
    return TRUE;
}

void start_simulation(GtkWidget *widget, gpointer data) {
    if (input_mode == 1) {
        // Mode 1: Generate random temperature and humidity readings
        generate_random_readings(readings, selected_log_size);
    } else {
        // Mode 2 or 3: Load data from CSV files
        // Mode 3: Test CSV (test_readings.csv) - user-generated test data
        const char* filename = (input_mode == 3) ? "test_readings.csv" : "readings.csv";
        strcpy(csv_filename, filename);
        
        if (!read_csv_readings(csv_filename, readings, selected_log_size)) {
            GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
                GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, 
                "CSV file error!\nMake sure '%s' exists and has enough data.", csv_filename);
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
            return;
        }
    }
    
    gtk_widget_set_sensitive(widget, FALSE);
    gtk_list_store_clear(list_store);
    
    has_current_iter = FALSE;
    
    // Dummy timeout
    g_timeout_add(500, simulate_reading, widget);
}

//From here code mainly focused on GUI styling and layout (our coding ends for main.c)

static void on_readings_count_changed(GtkSpinButton *spin_button, gpointer user_data) {
    selected_log_size = (int)gtk_spin_button_get_value(spin_button);
    printf("Number of readings set to: %d\n", selected_log_size);
}

static void on_dropdown_changed(GtkComboBox *combo, gpointer user_data) {
    input_mode = gtk_combo_box_get_active(combo) + 1;
    const char* mode_names[] = {"Random Generator", "Default CSV File", "Test CSV File"};
    printf("Input mode set to: %s\n", mode_names[input_mode - 1]);
    
    if (input_mode == 3) {
        gtk_spin_button_set_range(GTK_SPIN_BUTTON(readings_spinner), 1, 5);
        selected_log_size = 5;
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(readings_spinner), 5);
        printf("Readings automatically set to 5 for Test CSV File mode\n");
    } else {
        gtk_spin_button_set_range(GTK_SPIN_BUTTON(readings_spinner), 1, 50);
        selected_log_size = 50;
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(readings_spinner), 50);
        printf("Readings automatically set to 50 for %s mode\n", mode_names[input_mode - 1]);
    }
}

void build_gui() {
    GtkWidget *window, *main_vbox, *control_panel, *control_grid, *scrolled, *button, *dropdown, *header, *headerbar;
    GtkWidget *readings_label, *mode_label;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *col;

    gtk_init(NULL, NULL);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Temperature & Humidity Logger");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 600);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

#if GTK_CHECK_VERSION(3,10,0)
    headerbar = gtk_header_bar_new();
    gtk_header_bar_set_title(GTK_HEADER_BAR(headerbar), "Temperature & Humidity Logger");
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(headerbar), TRUE);
    gtk_window_set_titlebar(GTK_WINDOW(window), headerbar);
#endif

    main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(main_vbox), 15);
    gtk_container_add(GTK_CONTAINER(window), main_vbox);

#if !GTK_CHECK_VERSION(3,10,0)
    header = gtk_label_new("Temperature & Humidity Logger");
    gtk_box_pack_start(GTK_BOX(main_vbox), header, FALSE, FALSE, 0);
#endif
    control_panel = gtk_frame_new("Simulation Settings");
    gtk_box_pack_start(GTK_BOX(main_vbox), control_panel, FALSE, FALSE, 0);
    
    control_grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(control_grid), 15);
    gtk_grid_set_row_spacing(GTK_GRID(control_grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(control_grid), 15);
    gtk_container_add(GTK_CONTAINER(control_panel), control_grid);

    readings_label = gtk_label_new("Number of Readings:");
    gtk_grid_attach(GTK_GRID(control_grid), readings_label, 0, 0, 1, 1);
    
    readings_spinner = gtk_spin_button_new_with_range(1, 50, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(readings_spinner), selected_log_size);
    gtk_grid_attach(GTK_GRID(control_grid), readings_spinner, 1, 0, 1, 1);

    mode_label = gtk_label_new("Input Mode:");
    gtk_grid_attach(GTK_GRID(control_grid), mode_label, 0, 1, 1, 1);
    
    dropdown = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(dropdown), "Random Generator");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(dropdown), "Default CSV File");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(dropdown), "Test CSV File");
    gtk_combo_box_set_active(GTK_COMBO_BOX(dropdown), 0);
    gtk_grid_attach(GTK_GRID(control_grid), dropdown, 1, 1, 1, 1);

    button = gtk_button_new_with_label("Start Simulation");
    gtk_grid_attach(GTK_GRID(control_grid), button, 0, 2, 2, 1);

    list_store = gtk_list_store_new(6, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_store));
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree_view), TRUE);

    const char *titles[] = {"Seq\t\t", "Timestamp\t\t", "Temperature (C)", "Humidity (%)\t", "Temperature Status\t\t\t\t\t\t", "Humidity Status"};
    for (int i = 0; i < 6; i++) {
        renderer = gtk_cell_renderer_text_new();
        col = gtk_tree_view_column_new_with_attributes(titles[i], renderer, "text", i, NULL);
        
        if (i == 4) {
            gtk_tree_view_column_set_cell_data_func(col, renderer, temp_status_cell_data_func, NULL, NULL);
        } else if (i == 5) {
            gtk_tree_view_column_set_cell_data_func(col, renderer, humid_status_cell_data_func, NULL, NULL);
        } else {
            gtk_tree_view_column_set_cell_data_func(col, renderer, generic_cell_data_func, NULL, NULL);
        }
            
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), col);
    }

    scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled), tree_view);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scrolled, TRUE);
    gtk_widget_set_hexpand(scrolled, TRUE);
    gtk_box_pack_start(GTK_BOX(main_vbox), scrolled, TRUE, TRUE, 0);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(button, "clicked", G_CALLBACK(start_simulation), NULL);
    g_signal_connect(dropdown, "changed", G_CALLBACK(on_dropdown_changed), NULL);
    g_signal_connect(readings_spinner, "value-changed", G_CALLBACK(on_readings_count_changed), NULL);

    gtk_widget_show_all(window);
    gtk_main();
}


int main(int argc, char *argv[]) {
    printf("Default settings: %d readings, Random mode\n", selected_log_size);
    printf("Available input modes:\n");
    printf("  1. Random Generator\n");
    printf("  2. Default CSV File (readings.csv)\n");
    printf("  3. Test CSV File (test_readings.csv)\n\n");
    
    // Launch the graphical user interface
    build_gui();
    return 0;
}