﻿using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.ComponentModel;
using Profiler.Data;

namespace Profiler
{
	using SamplingBoard = Board<SamplingBoardItem, SamplingDescription, SamplingNode>;
	using EventBoard = Board<EventBoardItem, EventDescription, EventNode>;
	using System.Timers;
	using System.Collections;

	public delegate void ApplyFilterEventHandler(HashSet<Object> filter, FilterMode mode);
	public delegate void ApplyDescriptionFilterEventHandler(HashSet<Object> filter);

	/// <summary>
	/// Interaction logic for FrameDataTable.xaml
	/// </summary>
	public partial class FrameDataTable : UserControl
	{
		public FrameDataTable()
		{
			this.InitializeComponent();

			FilterText.DelayedTextChanged += new SearchBox.DelayedTextChangedEventHandler(UpdateTableFilter);
			FilterText.TextEnter += new SearchBox.TextEnterEventHandler(FilterText_TextEnter);
		}

		public static void ApplyAutogeneratedColumnAttributes(DataGridAutoGeneratingColumnEventArgs e)
		{
			PropertyDescriptor pd = e.PropertyDescriptor as PropertyDescriptor;
			if (pd.Attributes[typeof(HiddenColumn)] != null)
			{
				e.Cancel = true;
				return;
			}

			DisplayNameAttribute nameAttribute = pd.Attributes[typeof(DisplayNameAttribute)] as DisplayNameAttribute;
			if (nameAttribute != null && !String.IsNullOrEmpty(nameAttribute.DisplayName))
			{
				e.Column.Header = nameAttribute.DisplayName;
			}

			ColumnWidth columnWidth = pd.Attributes[typeof(ColumnWidth)] as ColumnWidth;
			if (columnWidth != null)
			{
				e.Column.Width = columnWidth.Width;
			}

			if (e.PropertyType == typeof(double))
			{
				(e.Column as DataGridTextColumn).Binding.StringFormat = "{0:0.###}";
			}

			if (e.PropertyType == typeof(bool) && !e.Column.IsReadOnly)
			{
				var checkboxFactory = new FrameworkElementFactory(typeof(CheckBox));
				checkboxFactory.SetValue(FrameworkElement.HorizontalAlignmentProperty, HorizontalAlignment.Center);
				checkboxFactory.SetValue(FrameworkElement.VerticalAlignmentProperty, VerticalAlignment.Center);
				checkboxFactory.SetBinding(CheckBox.IsCheckedProperty, new Binding(e.PropertyName) { UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged });
				e.Column = new DataGridTemplateColumn { Header = e.Column.Header, CellTemplate = new DataTemplate { VisualTree = checkboxFactory }, SortMemberPath = e.Column.SortMemberPath };
			}
		}

		private void SummaryTable_AutoGeneratingColumn(object sender, System.Windows.Controls.DataGridAutoGeneratingColumnEventArgs e)
		{
			ApplyAutogeneratedColumnAttributes(e);
		}

		void UpdateTableFilter(String text)
		{
			if (SummaryTable != null && SummaryTable.ItemsSource != null)
			{
				SummaryTable.SelectedItem = null;

				ICollectionView view = CollectionViewSource.GetDefaultView(SummaryTable.ItemsSource);

				if (String.IsNullOrWhiteSpace(text))
				{
					view.Filter = null;

					DescriptionFilterApplied(null);
				}
				else
				{
					view.Filter = new Predicate<object>((item) => { return (item is IBoardItem) ? (item as IBoardItem).Match(text) : true; });

					HashSet<Object> descriptions = new HashSet<Object>();

					foreach (object obj in view)
						if (obj is IBoardItem)
							(obj as IBoardItem).CollectDescriptionFilter(descriptions);

					DescriptionFilterApplied(descriptions);
				}
			}
		}

		public ApplyFilterEventHandler FilterApplied;
		public ApplyDescriptionFilterEventHandler DescriptionFilterApplied;


		private void FilterText_TextEnter(string text)
		{
			if (SummaryTable.ItemsSource != null)
			{
				ICollectionView view = CollectionViewSource.GetDefaultView(SummaryTable.ItemsSource);
				HashSet<Object> filter = null;

				if (!(view.SourceCollection is IList) || (view.SourceCollection as IList).Count != SummaryTable.Items.Count)
				{
					filter = new HashSet<object>();

					foreach (object obj in view)
						if (obj is IBoardItem)
							(obj as IBoardItem).CollectNodeFilter(filter);
				}

				FilterApplied(filter != null && filter.Count > 0 ? filter : null, new FilterMode() { ChangeExpand = true });
			}
		}

		public void RefreshFilter()
		{
			HashSet<Object> filter = new HashSet<Object>();

			if (SummaryTable == null)
				return;

			foreach (IBoardItem item in SummaryTable.SelectedItems)
				if (item != null)
					item.CollectNodeFilter(filter);

			if (FilterApplied != null)
			{
				FilterApplied(filter, new FilterMode() { ChangeExpand = true });
			}
		}

		private void SummaryTable_SelectionChanged(object sender, SelectionChangedEventArgs e)
		{
			if (e.AddedItems.Count == 0)
				return;

			RefreshFilter();
		}

		private void SummaryTable_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.Key == Key.Escape)
			{
				SummaryTable.UnselectAll();
				FilterApplied(null, new FilterMode());
			}
		}

		private void SummaryTable_Sorting(object sender, DataGridSortingEventArgs e)
		{
			if (e.Column.SortDirection == null)
			{
				ICollectionView view = CollectionViewSource.GetDefaultView(SummaryTable.ItemsSource);
				if (view != null)
				{
					view.SortDescriptions.Clear();
					view.SortDescriptions.Add(new SortDescription(e.Column.SortMemberPath, ListSortDirection.Descending));
					e.Column.SortDirection = ListSortDirection.Descending;
					e.Handled = true;
				}
			}
		}

		private void Row_DoubleClick(object sender, RoutedEventArgs e)
		{
			if (!(sender is DataGridRow))
				return;

			Object rowDataContext = (sender as DataGridRow).DataContext;

			Object windowDataContext = null;

			if (rowDataContext is SamplingBoardItem)
			{
				windowDataContext = SourceView<SamplingBoardItem, SamplingDescription, SamplingNode>.Create(DataContext as Board<SamplingBoardItem, SamplingDescription, SamplingNode>, (rowDataContext as SamplingBoardItem).Description.Path);
			}
			else if (rowDataContext is EventBoardItem)
			{
				windowDataContext = SourceView<EventBoardItem, EventDescription, EventNode>.Create(DataContext as Board<EventBoardItem, EventDescription, EventNode>, (rowDataContext as EventBoardItem).Description.Path);
			}

			if (windowDataContext != null)
			{
				new SourceWindow() { DataContext = windowDataContext }.Show();
			}
		}
	}
}