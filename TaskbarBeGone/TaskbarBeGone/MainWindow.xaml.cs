using Microsoft.UI.Xaml;
using System.Runtime.InteropServices;
using Windows.ApplicationModel.Background;

namespace TaskbarBeGone {
	public sealed partial class MainWindow : Window {
		[DllImport("TaskbarBeGone Win32 Api.dll", EntryPoint = "RegisterHotkey", CharSet = CharSet.Unicode)]
		public static extern void RegisterHotkey(int hwnd, int id, uint modifiers, char k);

		[DllImport("TaskbarBeGone Win32 Api.dll", EntryPoint = "UnregisterHotkey", CharSet = CharSet.Unicode)]
		public static extern void UnregisterHotkey();

		private uint modifiers;
		private bool taskRegistered = false;
		private string backgroundTask = "Task";
		BackgroundTaskRegistration bTask;

		public MainWindow() {
			this.InitializeComponent();
			ExtendsContentIntoTitleBar = true;
			SetTitleBar(TitleBar);
		}

		private void Apply_Click(object sender, RoutedEventArgs e) {
			// Toggle Visibility of Buttons
			Apply.Visibility = Visibility.Collapsed;
			Clear.Visibility = Visibility.Visible;

			// Construct Hotkey modifiers
			if (Shift.IsChecked == true) {
				modifiers |= 0x0004;
			}
			if (Ctrl.IsChecked == true) {
				modifiers |= 0x0002;
			}
			if (Alt.IsChecked == true) {
				modifiers |= 0x0001;
			}

			foreach (var task in BackgroundTaskRegistration.AllTasks) {
				if (task.Value.Name == backgroundTask) {
					taskRegistered = true;
					BackgroundTaskBuilder builder = new BackgroundTaskBuilder();

					builder.Name = backgroundTask;
					builder.TaskEntryPoint = "TaskbarBeGone_Background_Task.Task";
					builder.SetTrigger(new SystemTrigger(SystemTriggerType.SessionConnected, false));

					bTask = builder.Register();
					break;
				}
			}

			// Pass on hotkey to DLL to be registered with windows
			if (Key.Text != "") { RegisterHotkey(0, 0, modifiers, Key.Text.ToCharArray()[0]); }
		}

		private void Clear_Click(object sender, RoutedEventArgs e) {
			// Toggle Visibility of Buttons
			Apply.Visibility = Visibility.Visible;
			Clear.Visibility = Visibility.Collapsed;

			// Unregister Hotkey from windows
			UnregisterHotkey();

			// Clear Hotkey
			Shift.IsChecked = false;
			Ctrl.IsChecked = false;
			Alt.IsChecked = false;
			Key.Text = "";
		}
	}
}
