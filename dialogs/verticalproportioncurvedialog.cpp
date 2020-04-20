#include "verticalproportioncurvedialog.h"
#include "ui_verticalproportioncurvedialog.h"

#include "domain/application.h"
#include "domain/projectcomponent.h"
#include "domain/attribute.h"
#include "domain/project.h"
#include "domain/categorydefinition.h"
#include "domain/datafile.h"
#include "widgets/fileselectorwidget.h"
#include <QDragEnterEvent>
#include <QMimeData>

VerticalProportionCurveDialog::VerticalProportionCurveDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VerticalProportionCurveDialog)
{
    ui->setupUi(this);

    //deletes dialog from memory upon user closing it
    this->setAttribute(Qt::WA_DeleteOnClose);

    //enables this dialog as a drop destination in DnD operations
    setAcceptDrops(true);

    //add the fallback PDF file selector
    m_cmbFallBackPDF = new FileSelectorWidget( FileSelectorType::PDFs );
    ui->frmCmbFallbackPDF->layout()->addWidget( m_cmbFallBackPDF );

    //the top and base horizons selectors
    m_cmbTopHorizon = new FileSelectorWidget( FileSelectorType::CartesianGrids2D );
    ui->frmCmbTopHorizon->layout()->addWidget( m_cmbTopHorizon );
    m_cmbBaseHorizon = new FileSelectorWidget( FileSelectorType::CartesianGrids2D );
    ui->frmCmbBaseHorizon->layout()->addWidget( m_cmbBaseHorizon );

    setWindowTitle( "Create vertical proportion curves from data." );
}

VerticalProportionCurveDialog::~VerticalProportionCurveDialog()
{
    delete ui;
    Application::instance()->logInfo("VerticalProportionCurveDialog destroyed.");
}

void VerticalProportionCurveDialog::dragEnterEvent(QDragEnterEvent *e)
{
    e->acceptProposedAction();
}

void VerticalProportionCurveDialog::dragMoveEvent(QDragMoveEvent *e)
{
    QPoint eventPos = e->pos();

    //the variables list accepts drops if they come from somewhere other than lstVariables itself.
    if( ui->lstVariables->geometry().contains( eventPos ) && e->source() != ui->lstVariables ){
        m_dragOrigin = DragOrigin::FROM_ELSEWHERE;
        e->accept();
    //the trash bin label accepts drops if they come from the variables list.
    }else if( ui->lblTrash->geometry().contains( eventPos ) && e->source() == ui->lstVariables ){
        m_dragOrigin = DragOrigin::FROM_LIST_OF_VARIABLES;
        e->accept();
    }else {
        e->ignore();
        return;
    }

}

void VerticalProportionCurveDialog::dropEvent(QDropEvent *e)
{
    //user may be dropping data files to add to the project
    if (e->mimeData()->hasUrls()) {
        foreach (const QUrl &url, e->mimeData()->urls()) {
            QString fileName = url.toLocalFile();
            Application::instance()->addDataFile( fileName );
        }
        return;
    }

    //otherwise, they are project objects
    //inform user that an object was dropped
    QString object_locator = e->mimeData()->text();
    Application::instance()->logInfo("VerticalProportionCurveDialog::dropEvent(): Dropped object locator: " + object_locator);
    ProjectComponent* object = Application::instance()->getProject()->findObject( object_locator );
    if( ! object ){
        Application::instance()->logError("VerticalProportionCurveDialog::dropEvent(): object not found. Drop operation failed.");
    } else {
        Application::instance()->logInfo("VerticalProportionCurveDialog::dropEvent(): Found object: " + object->getName());
        if( object->isAttribute() ){
            Attribute* attributeAspect = dynamic_cast<Attribute*>( object );
            if( attributeAspect->isCategorical() ){
                //if dragging objects into the dialog
                if( m_dragOrigin == DragOrigin::FROM_ELSEWHERE ){
                    tryToAddAttribute( attributeAspect );
                } else { //if dragging objects from the dialog's list of variables to the trash bin
                    tryToRemoveAttribute( attributeAspect );
                }
            }else
                Application::instance()->logError("VerticalProportionCurveDialog::dropEvent(): attribute is not categorical.");
        } else
            Application::instance()->logError("VerticalProportionCurveDialog::dropEvent(): object is not an attribute.");
    }
}


void VerticalProportionCurveDialog::tryToAddAttribute(Attribute *attribute)
{
    CategoryDefinition* CDofFirst = nullptr;
    if( ! m_categoricalAttributes.empty() ){
        //get info from the first attribute added
        DataFile* parentOfFirst = dynamic_cast<DataFile*>( m_categoricalAttributes.front()->getContainingFile() );
        CDofFirst = parentOfFirst->getCategoryDefinition( m_categoricalAttributes.front() );

        //get info from the attribute to be added
        DataFile* myParent = dynamic_cast<DataFile*>( attribute->getContainingFile() );
        CategoryDefinition* myCD = myParent->getCategoryDefinition( attribute );

        //the category defininion must be the same
        if( CDofFirst != myCD ){
            Application::instance()->logError( "VerticalProportionCurveDialog::tryToAddAttribute(): all attributes must be associated to the same categorical definition.", true );
            return;
        }

        //it can't be added twice
        for( Attribute* at : m_categoricalAttributes ){
            if( at == attribute ){
                Application::instance()->logError( "VerticalProportionCurveDialog::tryToAddAttribute(): the attribute has already been added.", true );
                return;
            }
        }
    }

    //add the dragged attribute to the bookkeeping list
    m_categoricalAttributes.push_back( attribute );

    updateVariablesList();
}

void VerticalProportionCurveDialog::tryToRemoveAttribute(Attribute *attribute)
{
    std::vector<Attribute*>::iterator it;

    //add the dragged attribute from the bookkeeping list
    it = std::find(m_categoricalAttributes.begin(), m_categoricalAttributes.end(), attribute);
    if (it != m_categoricalAttributes.end())
        m_categoricalAttributes.erase( it );
    else
        Application::instance()->logError("VerticalProportionCurveDialog::tryToRemoveAttribute(): object pointer not found.  The dialog may be in an inconsistent state.");

    updateVariablesList();
}

void VerticalProportionCurveDialog::updateVariablesList()
{
    ui->lstVariables->clear();
    for( Attribute* attribute : m_categoricalAttributes ){
        QListWidgetItem* item = new QListWidgetItem( attribute->getIcon(), attribute->getName() );
        item->setData( Qt::UserRole, reinterpret_cast<uint64_t>( attribute ) );
        ui->lstVariables->addItem( item );
    }
}
