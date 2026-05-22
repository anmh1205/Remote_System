#include "MainWindow.h"
#include <QApplication>

/**
 * @brief Application entry point
 * 
 * This is the main entry point of the Remote System application.
 * The initialization flow is as follows:
 * 
 * 1. Create QApplication - This is the core Qt object that manages the event loop,
 *    handles system events, and provides application-wide settings.
 * 
 * 2. Set application metadata - These values are used for settings storage,
 *    window titles, and system integration.
 * 
 * 3. Create MainWindow - The main window constructor will:
 *    - Create QStackedWidget for view navigation
 *    - Create all three views (MainControlView, WifiInfoView, IpConfigView)
 *    - Create MainController which initializes all models and controllers
 *    - Connect all signals/slots between components
 * 
 * 4. Show window - Makes the window visible (but not yet displayed until event loop starts)
 * 
 * 5. Start event loop - app.exec() blocks until application exits.
 *    This is when the application actually starts running and processing events.
 */
int main(int argc, char *argv[])
{
    // Step 1: Create QApplication - Core Qt object managing event loop and system integration
    QApplication app(argc, argv);
    
    // Step 2: Set application metadata for settings storage and system integration
    app.setApplicationName("Remote System");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("RemoteSystem");
    
    // Step 3: Create MainWindow - This triggers the entire initialization chain:
    //   - MainWindow constructor creates UI components
    //   - MainController constructor creates models and controllers
    //   - All signal/slot connections are established
    MainWindow window;
    
    // Step 4: Show window (will be displayed when event loop starts)
    window.show();
    
    // Step 5: Start Qt event loop - Application begins processing events
    // This function blocks until the application is closed
    // All user interactions, network events, and timers are processed here
    return app.exec();
}

