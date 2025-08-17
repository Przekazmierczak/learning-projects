using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class ButtonSetup : MonoBehaviour
{
    private Button button;
    public string sceneToLoad;
    public int difficulty;

    void Awake()
    {
        // Get the Button component on the same GameObject
        button = GetComponent<Button>();
    }

    void Start()
    {
        button.onClick.AddListener(OnButtonClick);
    }

    // This function will be called when button is clicked
    public void OnButtonClick()
    {
        if (GameManager.Instance != null)
        {
            GameManager.Instance.difficulty = difficulty;
        }
        SceneManager.LoadScene(sceneToLoad);
    }
}
