#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simulator.h"
#include "stats.h"

#define LOG_SIZE 50
#define ALERT_THRESHOLD 65
#define WARNING_THRESHOLD 60

Reading readings[LOG_SIZE];
Statistics stats;

static GtkListStore *list_store;
static GtkWidget *alert_label;
static GtkCssProvider *css_provider;

// Cell renderer for Temperature Status column
void temp_status_cell_data_func(GtkTreeViewColumn *col, GtkCellRenderer *renderer,
                               GtkTreeModel *model, GtkTreeIter *iter, gpointer data) {
    gchar *status;
    gtk_tree_model_get(model, iter, 4, &status, -1);
    
    const char *color;
    if (strstr(status, "Alert!") != NULL) {
        color = "#dc3545"; // Red for alert
    } else if (strstr(status, "Warning!") != NULL) {
        color = "#ffc107"; // Yellow for warning
    } else {
        color = "#28a745"; // Green for safe
    }
    
    g_object_set(renderer,
        "foreground", color,
        "weight", PANGO_WEIGHT_BOLD,
        NULL);
    g_free(status);
}

// Cell renderer for Humidity Status column
void humid_status_cell_data_func(GtkTreeViewColumn *col, GtkCellRenderer *renderer,
                               GtkTreeModel *model, GtkTreeIter *iter, gpointer data) {
    gchar *status;
    gtk_tree_model_get(model, iter, 5, &status, -1);
    
    const char *color;
    if (strstr(status, "Alert!") != NULL) {
        color = "#dc3545"; // Red for alert
    } else if (strstr(status, "Warning!") != NULL) {
        color = "#ffc107"; // Yellow for warning
    } else {
        color = "#28a745"; // Green for safe
    }
    
    g_object_set(renderer,
        "foreground", color,
        "weight", PANGO_WEIGHT_BOLD,
        NULL);
    g_free(status);
}

// Update alert label styling based on status
void set_alert_style(const char* status_temp, const char* status_humid) {
    GtkStyleContext *ctx = gtk_widget_get_style_context(alert_label);
    
    // Remove all existing classes
    gtk_style_context_remove_class(ctx, "alert-safe");
    gtk_style_context_remove_class(ctx, "alert-warning");
    gtk_style_context_remove_class(ctx, "alert-danger");
    
    // Check for alert conditions first (highest priority)
    if (strstr(status_temp, "Alert!") || strstr(status_humid, "Alert!")) {
        gtk_style_context_add_class(ctx, "alert-danger");
    }
    // Then check for warning conditions
    else if (strstr(status_temp, "Warning!") || strstr(status_humid, "Warning!")) {
        gtk_style_context_add_class(ctx, "alert-warning");
    }
    // Otherwise it's safe
    else {
        gtk_style_context_add_class(ctx, "alert-safe");
    }
}

void update_gui(int idx) {
    GtkTreeIter iter;
    gtk_list_store_append(list_store, &iter);

    char temp_str[32], humid_str[32], safe_temp[50], safe_humid[50];
    sprintf(temp_str, "%.2f °C", readings[idx].temperature);
    sprintf(humid_str, "%.2f %%", readings[idx].humidity);

    // Temperature status logic
    if (readings[idx].temperature > ALERT_THRESHOLD)
        strcpy(safe_temp, "Alert! Safe Temperature exceeded.");
    else if (readings[idx].temperature >= WARNING_THRESHOLD)
        strcpy(safe_temp, "Warning! Safe Temperature exceeding.");
    else
        strcpy(safe_temp, "Safe Temperature levels.");

    // Humidity status logic
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

    char markup_msg[256];
    sprintf(markup_msg, "<b>%s | %s</b>", safe_temp, safe_humid);
    gtk_label_set_markup(GTK_LABEL(alert_label), markup_msg);

    set_alert_style(safe_temp, safe_humid);
}

gboolean simulate_reading(gpointer data) {
    static int idx = 0;
    if (idx >= LOG_SIZE) {
        calc_statistics(readings, LOG_SIZE, &stats);
        char stats_msg[256];
        sprintf(stats_msg,
            "Average Temp: %.2f °C\nAverage Humid: %.2f %%\n"
            "Max Temp: %.2f °C at %s\n"
            "Max Humid: %.2f %% at %s\n",
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
    gtk_list_store_clear(list_store);
    gtk_label_set_markup(GTK_LABEL(alert_label), "<b>Simulating...</b>");
    
    // Remove all alert classes during simulation
    GtkStyleContext *ctx = gtk_widget_get_style_context(alert_label);
    gtk_style_context_remove_class(ctx, "alert-danger");
    gtk_style_context_remove_class(ctx, "alert-warning");
    gtk_style_context_remove_class(ctx, "alert-safe");
    
    // Reset simulation index
    void reset_simulate_reading_index(void);
    reset_simulate_reading_index();
    g_timeout_add(500, simulate_reading, NULL);
}

void reset_simulate_reading_index(void) {
    // Function to reset static index in simulate_reading
}

static void on_dropdown_changed(GtkComboBox *combo, gpointer mode_ptr) {
    *(int*)mode_ptr = gtk_combo_box_get_active(combo) + 1;
}

void apply_css(GtkWidget *window) {
    css_provider = gtk_css_provider_new();
    const gchar *css =
        /* Simple fixed color background */
        "window { "
        "  background-color: #f0f0f0; "
        "} "
        
        /* Header styling */
        ".header-bar { "
        "  background-color: #2c3e50; "
        "  color: white; "
        "  font-size: 20px; "
        "  font-weight: bold; "
        "} "
        
        ".header-label { "
        "  font-size: 24px; "
        "  font-weight: bold; "
        "  color: #2c3e50; "
        "  padding: 15px; "
        "} "
        
        /* TreeView styling */
        "treeview { "
        "  background-color: white; "
        "  color: #333; "
        "  font-size: 14px; "
        "} "
        
        "treeview header button { "
        "  background-color: #3498db; "
        "  color: white; "
        "  font-weight: bold; "
        "} "
        
        /* Button styling */
        "button { "
        "  background-color: #3498db; "
        "  color: white; "
        "  border-radius: 5px; "
        "  padding: 10px 20px; "
        "  font-weight: bold; "
        "  border: none; "
        "} "
        
        "button:hover { "
        "  background-color: #2980b9; "
        "} "
        
        /* ComboBox styling */
        "combobox { "
        "  font-size: 14px; "
        "  background-color: white; "
        "} "
        
        /* Label styling */
        "label { "
        "  font-size: 14px; "
        "  color: #333; "
        "} "
        
        /* Alert styling */
        ".alert-danger { "
        "  color: #dc3545; "
        "  font-weight: bold; "
        "  font-size: 16px; "
        "  background-color: #f8d7da; "
        "  padding: 10px; "
        "  border: 1px solid #f5c6cb; "
        "  border-radius: 5px; "
        "} "
        
        ".alert-warning { "
        "  color: #856404; "
        "  font-weight: bold; "
        "  font-size: 16px; "
        "  background-color: #fff3cd; "
        "  padding: 10px; "
        "  border: 1px solid #ffeaa7; "
        "  border-radius: 5px; "
        "} "
        
        ".alert-safe { "
        "  color: #155724; "
        "  font-weight: bold; "
        "  font-size: 16px; "
        "  background-color: #d4edda; "
        "  padding: 10px; "
        "  border: 1px solid #c3e6cb; "
        "  border-radius: 5px; "
        "}";
        
    gtk_css_provider_load_from_data(css_provider, css, -1, NULL);

    GtkStyleContext *context = gtk_widget_get_style_context(window);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}

void build_gui(int *input_mode) {
    GtkWidget *window, *vbox, *scrolled, *tree, *button, *dropdown, *header, *headerbar;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *col;

    gtk_init(NULL, NULL);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Temperature & Humidity Logger");
    gtk_window_set_default_size(GTK_WINDOW(window), 850, 500);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    apply_css(window);

    // HeaderBar
#if GTK_CHECK_VERSION(3,10,0)
    headerbar = gtk_header_bar_new();
    gtk_header_bar_set_title(GTK_HEADER_BAR(headerbar), "Temperature & Humidity Logger");
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(headerbar), TRUE);
    gtk_widget_set_name(headerbar, "header-bar");
    gtk_window_set_titlebar(GTK_WINDOW(window), headerbar);
#endif

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 15);
    gtk_container_add(GTK_CONTAINER(window), vbox);

#if !GTK_CHECK_VERSION(3,10,0)
    // Add fallback header if headerbar not available
    header = gtk_label_new("Temperature & Humidity Logger");
    gtk_widget_set_name(header, "header-label");
    gtk_box_pack_start(GTK_BOX(vbox), header, FALSE, FALSE, 0);
#endif

    // Input selection dropdown
    dropdown = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(dropdown), "Random Generator");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(dropdown), "CSV File Input");
    gtk_combo_box_set_active(GTK_COMBO_BOX(dropdown), 0);
    gtk_box_pack_start(GTK_BOX(vbox), dropdown, FALSE, FALSE, 0);

    // Start simulation button
    button = gtk_button_new_with_label("Start Simulation");
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);

    // Data table setup
    list_store = gtk_list_store_new(6, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_store));
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree), TRUE);

    const char *titles[] = {"Seq No", "Timestamp", "Temperature", "Humidity", "Temp Status", "Humid Status"};
    for (int i = 0; i < 6; i++) {
        renderer = gtk_cell_renderer_text_new();
        col = gtk_tree_view_column_new_with_attributes(titles[i], renderer, "text", i, NULL);
        
        if (i == 4)
            gtk_tree_view_column_set_cell_data_func(col, renderer, temp_status_cell_data_func, NULL, NULL);
        if (i == 5)
            gtk_tree_view_column_set_cell_data_func(col, renderer, humid_status_cell_data_func, NULL, NULL);
            
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree), col);
    }

    // Scrolled window for the data table
    scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled), tree);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scrolled, TRUE);
    gtk_widget_set_hexpand(scrolled, TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);

    // Status alert label
    alert_label = gtk_label_new("Waiting for simulation...");
    gtk_label_set_use_markup(GTK_LABEL(alert_label), TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), alert_label, FALSE, FALSE, 10);

    // Connect signals
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