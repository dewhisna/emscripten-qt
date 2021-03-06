#ifndef HTML5CANVAS_INTERFACE_H
#define HTML5CANVAS_INTERFACE_H

#include <QtCore/qglobal.h>
#include <QtGui/qimage.h>
QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

typedef qint32 CanvasHandle;
typedef quint32 Rgba;

class Q_GUI_EXPORT Html5CanvasInterface
{
public:
    /**
     * Get the handle for the Canvas element in the DOM that everything ends up being rendered to.
     * Returns -1 if no such Canvas element cannot be found.
     */
    static CanvasHandle handleForMainCanvas();

    static int mainCanvasWidth();

    static int mainCanvasHeight();

    /**
     * Create a Canvas (offscreen) off the given \a width and \a height, and return a handle to it,
     * or -1 if it could not be created.
     */
    static CanvasHandle createCanvas(int width, int height);
    /**
     * Fill the region specified by \a x, \a y, \a width and \a height with the given rgb values.
     * The canvas state (in particular, the fillStyle) is not affected by this method.
     */
    static void fillSolidRect(CanvasHandle canvasHandle, int r, int g, int b, double x, double y, double width, double height);
    /**
     * Draw (but don't fill) the given rect with the current pen. TODO - explain (after figuring out!) how pen thickness is handled, etc.
     */
    static void strokeRect(CanvasHandle canvasHandle, double x, double y, double width, double height);
    /**
     * Fill (without outline) the given rect with the current brush.
     */
    static void fillRect(CanvasHandle canvasHandle, double x, double y, double width, double height);
    /**
     * Draw (but don't fill) the given ellipse with the current pen. TODO - explain (after figuring out!) how pen thickness is handled, etc.
     */
    static void strokeEllipse(CanvasHandle canvasHandle, double cx, double cy, double width, double height);
    /**
     * Fill (without outline) the given ellipse with the current pen.
     */
    static void fillEllipse(CanvasHandle canvasHandle, double cx, double cy, double width, double height);
    static void drawLine(CanvasHandle canvasHandle, double startX, double startY, double endX, double endY);
    /**
     * Draw the canvas with the requested \a canvasHandle on the main canvas at \a x, \a y
     */
    static void changePenColor(CanvasHandle canvasHandle, int r, int g, int b);
    static void changeBrushColor(CanvasHandle canvasHandle, int r, int g, int b);
    static void changeBrushTexture(CanvasHandle canvasHandle, CanvasHandle textureHandle);
    static void changePenThickness(CanvasHandle canvasHandle, double thickness);
    /**
     * Create a linear gradient with the given start and end points.  This will be set to the <em>current</em>
     * gradient, which can be modified via calls to #addStopPointToCurrentGradient and set as a brush via
     * #setBrushToCurrentGradient.
     * The \a canvasHandle should be the same as that used when you call setBrushToCurrentGradient (HTML5
     * canvas requires this knowledge upfront).
     */
    static void createLinearGradient(CanvasHandle canvasHandle, double startX, double startY, double endX, double endY);
    static void addStopPointToCurrentGradient(double position, int r, int g, int b);
    static void setBrushToCurrentGradient(CanvasHandle canvasHandle);
    static void drawCanvasOnMainCanvas(CanvasHandle canvasHandle, int x, int y);
    static void drawCanvasOnCanvas(CanvasHandle canvasHandleToDraw,CanvasHandle canvasHandleToDrawOn, double x, double y);
    static void drawStretchedCanvasPortionOnCanvas(CanvasHandle canvasHandleToDraw, CanvasHandle canvasHandleToDrawOn, double targetX, double targetY, double targetWidth, double targetHeight, double sourceX, double sourceY, double sourceWidth, double sourceHeight);
    static void savePaintState(CanvasHandle canvasHandle);
    static void restorePaintState(CanvasHandle canvasHandle);
    static void restoreToOriginalState(CanvasHandle canvasHandle);
    static void setClipRect(CanvasHandle canvasHandle, double x, double y, double w, double h);
    static void removeClip(CanvasHandle canvasHandle);

    static void beginPath(CanvasHandle canvasHandle);
    static void currentPathMoveTo(double x, double y);
    static void currentPathLineTo(double x, double y);
    static void currentPathCubicTo(double context1X, double context1Y, double context2X, double context2Y, double endX, double endY);
    static void addRectToCurrentPath(double x, double y, double width, double height);
    static void setClipToCurrentPath();
    static void strokeCurrentPath();
    static void fillCurrentPath();
    /**
     * Effectively calls the HTML5 Canvas method setTransform(a, b, c, d, e, f) method on \a canvasHandle 's context.
     * See http://www.whatwg.org/specs/web-apps/current-work/multipage/the-canvas-element.html#dom-context-2d-transform
     * for more information.
     */
    static void setTransform(CanvasHandle canvasHandle, double a, double b, double c, double d, double e, double f);
    /**
     * Make all subsequent operations be rotated clockwise about origin by \a rotationDegrees
     */
    static void setCanvasPixelsRaw(CanvasHandle canvasHandle, uchar* rgbaData, int width, int height);
    /**
     * Fill the main canvas with the colour \a rgba.  Mainly used by the html5canvas test suite.
     */
    static void clearMainCanvas(Rgba rgba);
    /**
     * Retrieve the contents of the main canvas as  a QImage. Mainly used by the html5canvas test suite.
     */
    static QImage mainCanvasContents();
    static void loadFont(const char* fontData, int fontDataSize, const char* familyName);
#ifdef EMSCRIPTEN_NATIVE
    // Tells the qtwebkit server to process events in order to e.g flush the canvas to screen.
    // This is only really needed if you are testing an app with animations and want to inspect it
    // visually.
    static void processEvents();
#endif
private:
    static Rgba* mainCanvasContentsRaw();
};

QT_END_NAMESPACE

QT_END_HEADER

#endif