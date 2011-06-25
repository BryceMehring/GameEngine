void main()
{
	UIData@ data = UIData();

	data.name = "Enter Here: ";
	data.rect.left = 0;
	data.rect.top = 500;
	data.rect.right = 600;
	data.rect.bottom = 600;

	ui.AddElement("TextBox",@data);
}