using Microsoft.UI;
using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;
using System;
using System.Runtime.InteropServices;
using Windows.Graphics;

namespace TaskbarBeGone {
	public partial class App : Application {
		[DllImport("user32.dll")]
		public static extern int ShowWindow(int hwnd, int nCmdShow);

		[DllImport("TaskbarBeGone Win32 Api.dll", EntryPoint = "RegisterInstance", CharSet = CharSet.Unicode)]
		public static extern void RegisterInstance(IntPtr hInstance);

		private AppWindow appW = null;
		private OverlappedPresenter presenter = null;
		private Window m_window;

		public App() {
			this.InitializeComponent();
		}

		protected override void OnLaunched(Microsoft.UI.Xaml.LaunchActivatedEventArgs args) {
			m_window = new MainWindow();

			IntPtr hwnd = WinRT.Interop.WindowNative.GetWindowHandle(m_window);
			WindowId wndId = Microsoft.UI.Win32Interop.GetWindowIdFromWindow(hwnd);
			appW = AppWindow.GetFromWindowId(wndId);
			presenter = appW.Presenter as OverlappedPresenter;
			presenter.IsResizable = false;

			SetWindowSize(460, 100);
			m_window.Activate();

			RegisterInstance(hwnd);
		}

		private void SetWindowSize(int width, int height) {
			appW.Resize(new SizeInt32(width, height));
		}

	}
}
