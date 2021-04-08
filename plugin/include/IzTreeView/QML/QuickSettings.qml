import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import IzLibrary 1.0
import IzTreeView 1.0

Control {
	id: root

	property int selectedNodeSize
	property int customNodeWidth
	property int customNodeHeight
	property int customColumnSpacing
	property int customRowSpacing
	property bool quickSettingsEnabled: true

	hoverEnabled: true

	background: Rectangle {
		color: Material.background
		border.color: Material.accent
		border.width: 1
		opacity: root.hovered ? 1.0 : 0.18
	}

	contentItem: ColumnLayout {
		id: mainLayout

		opacity: root.hovered ? 1.0 : 0.18

		IzText {
			Layout.fillWidth: true
			Layout.leftMargin: 4
			Layout.rightMargin: 4
			Layout.topMargin: 4

			text: qsTr("Ustawienia widoku")
			font.bold: true
		}

		RowLayout {
			Layout.fillWidth: true
			Layout.leftMargin: 4
			Layout.rightMargin: 4

			IzText {
				Layout.preferredWidth: 85
				text: qsTr("Odstępy wys.:")
			}

			IzTextField {
				Layout.fillWidth: true
				Layout.preferredHeight: 25

				placeholderText: qsTr("Odstępy wysokości")
				validator: IntValidator { bottom: 1; top: 100; }

				onEditingFinished: {
					root.customRowSpacing = text;
				}

				Component.onCompleted: {
					text = root.customRowSpacing;
				}
			}

			IzText {
				Layout.preferredWidth: 80
				text: qsTr("Odstępy sze.:")
			}

			IzTextField {
				Layout.fillWidth: true
				Layout.preferredHeight: 25

				placeholderText: qsTr("Odstępy szerokości")
				validator: IntValidator { bottom: 1; top: 100; }

				onEditingFinished: {
					root.customColumnSpacing = text;
				}

				Component.onCompleted: {
					text = root.customColumnSpacing;
				}
			}
		}

		RowLayout {
			Layout.fillWidth: true
			Layout.leftMargin: 4
			Layout.rightMargin: 4

			IzText {
				Layout.preferredWidth: 85
				text: qsTr("Węzły:")
			}

			IzComboBox {
				Layout.preferredHeight: 25
				Layout.fillWidth: true

				textRole: "description"
				model: ListModel {
					ListElement {
						description: qsTr("Małe")
						value: IzTreeNodeSize.Small
					}

					ListElement {
						description: qsTr("Średnie")
						value: IzTreeNodeSize.Medium
					}

					ListElement {
						description: qsTr("Duże")
						value: IzTreeNodeSize.Large
					}

					ListElement {
						description: qsTr("Użytkownika")
						value: IzTreeNodeSize.Custom
					}
				}

				onActivated: {
					root.selectedNodeSize = model.get(currentIndex).value;
				}

				Component.onCompleted: {
					if (root.quickSettingsEnabled) {
						for (var i = 0; i < model.count; ++i) {
							if (model.get(i).value === root.selectedNodeSize) {
								currentIndex = i;
								return;
							}
						}
					}
				}
			}
		}

		RowLayout {
			Layout.fillWidth: true
			Layout.leftMargin: 4
			Layout.rightMargin: 4

			visible: root.selectedNodeSize === IzTreeNodeSize.Custom

			IzText {
				Layout.preferredWidth: 85
				text: qsTr("Wysokość:")
			}

			IzTextField {
				Layout.fillWidth: true
				Layout.preferredHeight: 25

				placeholderText: qsTr("Wysokość")
				validator: IntValidator { bottom: 10; top: 700; }

				onEditingFinished: {
					root.customNodeHeight = text;
				}

				Component.onCompleted: {
					text = root.customNodeHeight;
				}
			}

			IzText {
				Layout.preferredWidth: 85
				text: qsTr("Szerokość:")
			}

			IzTextField {
				Layout.fillWidth: true
				Layout.preferredHeight: 25

				placeholderText: qsTr("Szerokość")
				validator: IntValidator { bottom: 10; top: 700; }

				onEditingFinished: {
					root.customNodeWidth = text;
				}

				Component.onCompleted: {
					text = root.customNodeWidth;
				}
			}
		}

		Item {
			Layout.fillHeight: true
			Layout.fillWidth: true
		}
	}
}
