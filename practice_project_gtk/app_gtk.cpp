#include <gtk/gtk.h>
#include <atomic>
#include <pthread.h>
// Atomic flags for thread control
std::atomic<bool> Recording(false);
static GtkWidget *status_label;
pthread_t recordThread;
FILE *fp = nullptr; // used for popen


void* recordAudio(void* arg) {
    // This function will only run once as per your updated logic
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW((GtkWidget *)arg));
    Recording = true;

    // Execute the command to print time once
    fp = popen("./example_func/print_time", "r");
    if (!fp) {
        g_print("Failed to run command\n");
        return nullptr;
    }

    char line[256];
    while (Recording && fgets(line, sizeof(line), fp) != NULL) {
        // Append line to GtkTextBuffer
        GtkTextIter end;
        gtk_text_buffer_get_end_iter(buffer, &end);
        gtk_text_buffer_insert(buffer, &end, line, -1);
    }
    pclose(fp);
    Recording = false;
    return nullptr;
}

static void start_recording(GtkWidget *widget, gpointer data) {
    GtkWidget *results_area = (GtkWidget *)data;
    

    if (!Recording) 
    {
        g_print("start recording!!\n");
        gtk_label_set_text(GTK_LABEL(status_label), "Status: Recording");
        pthread_create(&recordThread, nullptr, recordAudio, results_area);
    }
    else
    {
        g_print("It already starts recording.!!\n");
    }
}

static void stop_recording(GtkWidget *widget, gpointer data) {
    if (!Recording) {
        g_print("Recording is already stopped!\n");
        return;
    }

    g_print("Stopping recording...\n");
    Recording = false; // Signal the recording thread to stop

    // Cancel the recording thread (terminate it)
    if (recordThread) {
        pthread_cancel(recordThread);
        pthread_join(recordThread, nullptr); // Wait for the thread to finish
    }
    
    gtk_label_set_text(GTK_LABEL(status_label), "Status: Idle");

    g_print("Recording stopped successfully.\n");
}

static void clear_window_result(GtkWidget *widget, gpointer data) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW((GtkWidget *)data));
    gtk_text_buffer_set_text(buffer, "", -1);
    g_print("Results cleared!\n");
}
// Function to clean up resources, terminate threads and processes
/*void cleanup_and_quit(GtkWidget *widget, gpointer data) {
    // 1. Stop the recording thread if it's running
    if (Recording) {
        g_print("Stopping recording...\n");
        Recording = false; // Signal the recording thread to stop

        // Wait for the recording thread to finish
        // Cancel the recording thread (terminate it)
        if (recordThread) {
            g_print("SAAAtopping recording...\n");
            pthread_cancel(recordThread);
            g_print("CCCStopping recording...\n");
            pthread_join(recordThread, nullptr); // Wait for the thread to finish
            g_print("AAAASSSSStopping recording...\n");
        }
    }

    // 2. Close the pipe (child process) if it's running
    if (fp) {
        pclose(fp);
        fp = nullptr;
    }
    // 3. Quit the GTK application
    gtk_main_quit();  // This will stop the GTK main loop and close the application
    // 3. Destroy the GTK window (quit the application)
    //gtk_widget_destroy(GTK_WIDGET(data));
    //g_application_quit(G_APPLICATION(data));
}*/
/*
box -contain-> text_window and grid
*/
static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *box;        // Outer box container
    GtkWidget *results_area; // Text view for displaying results
    GtkWidget *grid;       // Grid for buttons
    GtkWidget *button;
    

    // Create the main application window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Welcome To Undersea Sound Detection");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

    // Create a box container to hold the results area and the grid
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10); // Vertical box with spacing
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_container_add(GTK_CONTAINER(window), box);

    // Create a text view for results area
    results_area = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(results_area), GTK_WRAP_WORD);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(results_area), FALSE);
    // Set fixed width and height
    //gtk_widget_set_size_request(results_area, 300, 200);  // width = 600px, height = 400px
    // Add a scrollable container for the results area
    GtkWidget *scroll_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll_window), results_area);
    // Optionally, set the initial size of the scroll window
    gtk_widget_set_size_request(scroll_window, 500, 400); // Same size as the text view
    // Make the scroll window expand to fill the available space
    gtk_widget_set_vexpand(scroll_window, TRUE);
    gtk_widget_set_hexpand(scroll_window, TRUE);
    
    gtk_box_pack_start(GTK_BOX(box), scroll_window, TRUE, TRUE, 0);

    // Add the status label below the buttons
    status_label = gtk_label_new("Status: Idle");
    gtk_box_pack_start(GTK_BOX(box), status_label, FALSE, FALSE, 0);
    
    // Create a grid layout for buttons
    grid = gtk_grid_new();
    
    // Make all rows and columns in the grid homogeneous, meaning they will all have the same size
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);

    
    gtk_box_pack_start(GTK_BOX(box), grid, FALSE, FALSE, 0);

    // Add buttons to the grid
    button = gtk_button_new_with_label("Start Recording");
    g_signal_connect(button, "clicked", G_CALLBACK(start_recording), results_area);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 0, 1, 1);

    button = gtk_button_new_with_label("Stop Recording");
    g_signal_connect(button, "clicked", G_CALLBACK(stop_recording), results_area);
    gtk_grid_attach(GTK_GRID(grid), button, 1, 0, 1, 1);

    button = gtk_button_new_with_label("Clear Result");
    g_signal_connect(button, "clicked", G_CALLBACK(clear_window_result), results_area);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 1, 2, 1);


    // Add the status label below the buttons
    GtkWidget *remind_label = gtk_label_new("Make sure click \"Stop Recording\" before \"Quit\"");
    gtk_box_pack_start(GTK_BOX(box), remind_label, FALSE, FALSE, 0);
    
    button = gtk_button_new_with_label("Quit");
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_widget_destroy), window);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 2, 2, 1);

    // Show all widgets
    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
